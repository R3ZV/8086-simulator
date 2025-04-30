const std = @import("std");
const ArrayList = std.ArrayList;

const Self = @This();

const DecoderErr = error{
    MissingMovOperand,
    MissingLoadImmediatData,
    InvalidReg,
    InvalidOpcode,
};

const Opcode = enum(u8) {
    RegMemToFromReg = 0b1000_1000,
    ImmediatToReg = 0b1011_0000,
};

instructions: []u8,
alloc: std.mem.Allocator,

pub fn init(alloc: std.mem.Allocator) Self {
    return Self{
        .alloc = alloc,
        .instructions = undefined,
    };
}

pub fn load(self: *Self, path: []const u8) !void {
    self.instructions = try std.fs.cwd().readFileAlloc(self.alloc, path, 8 * 1024);
}

pub fn free(self: *Self) void {
    self.alloc.free(self.instructions);
}

fn decode_reg(reg: u8, wide: bool) !*const [2:0]u8 {
    switch (reg) {
        0b000 => {
            if (wide) {
                return "ax";
            }
            return "al";
        },
        0b001 => {
            if (wide) {
                return "cx";
            }
            return "cl";
        },
        0b010 => {
            if (wide) {
                return "dx";
            }
            return "dl";
        },
        0b011 => {
            if (wide) {
                return "bx";
            }
            return "bl";
        },
        0b100 => {
            if (wide) {
                return "sp";
            }
            return "ah";
        },
        0b101 => {
            if (wide) {
                return "bp";
            }
            return "ch";
        },
        0b110 => {
            if (wide) {
                return "si";
            }
            return "dh";
        },
        0b111 => {
            if (wide) {
                return "di";
            }
            return "bh";
        },
        else => return DecoderErr.InvalidReg,
    }
}

fn decode_effective_addr(addr: u8, wide: bool) *const [2:0]u8 {
    _ = addr;
    _ = wide;
    unreachable;
}

pub fn decode(self: *const Self) ![]u8 {
    var decoded = ArrayList(u8).init(self.alloc);
    defer decoded.deinit();

    try decoded.appendSlice("bits 16\n");

    var i: usize = 0;
    while (i < self.instructions.len) : (i += 1) {
        const opcode = self.instructions[i];

        if (opcode & @intFromEnum(Opcode.ImmediatToReg) == @intFromEnum(Opcode.ImmediatToReg)) {
            if (i + 1 >= self.instructions.len) {
                return DecoderErr.MissingLoadImmediatData;
            }
            i += 1;
            const data_lo = self.instructions[i];

            const wide = (opcode & 0b0000_1000) > 0;

            if (wide) {
                if (i + 1 >= self.instructions.len) {
                    return DecoderErr.MissingLoadImmediatData;
                }
                i += 1;
            }

            const data_hi: u16 = if (wide)
                self.instructions[i]
            else
                0;

            const reg = try decode_reg(opcode & 0b0000_0111, wide);

            const immediat: u16 = data_lo + (data_hi << 8);
            const fmt = try std.fmt.allocPrint(self.alloc, "\nmov {s}, {d}", .{ reg, immediat });
            defer self.alloc.free(fmt);

            try decoded.appendSlice(fmt);
        } else if (opcode & @intFromEnum(Opcode.RegMemToFromReg) == @intFromEnum(Opcode.RegMemToFromReg,)) {
            if (i + 1 >= self.instructions.len) {
                return DecoderErr.MissingMovOperand;
            }

            i += 1;
            const operand = self.instructions[i];

            // 1 for word, 0 for byte
            const wide = (opcode & 0b0000_0001) > 0;

            const reg: u8 = (operand & 0b0011_1000) >> 3;
            const reg_field1 = try decode_reg(reg, wide);

            const reg_or_mem: u8 = operand & 0b0000_0111;

            // 0 means that REG is the source
            // 1 means that REG is the destination
            const direction = (opcode & 0b0000_0010) > 0;
            const reg_field2 = if (direction)
                decode_effective_addr(reg_or_mem, wide)
            else
                try decode_reg(reg_or_mem, wide);

            // const mod = (operand & 0b11000000) >> 6;

            const fmt = try std.fmt.allocPrint(self.alloc, "\nmov {s}, {s}", .{ reg_field2, reg_field1 });
            defer self.alloc.free(fmt);

            try decoded.appendSlice(fmt);
        } else {
            return DecoderErr.InvalidOpcode;
        }
    }

    return self.alloc.dupe(u8, decoded.items);
}

const testing = std.testing;
test "single_register_mov" {
    const alloc = std.testing.allocator;
    var decoder = Self.init(alloc);

    try decoder.load("./tests/single_register_mov");
    defer decoder.free();

    const res = try decoder.decode();
    defer alloc.free(res);

    const expected =
        \\bits 16
        \\
        \\mov cx, bx
    ;

    try testing.expectEqualDeep(expected, res);
}

test "many_register_mov" {
    const alloc = std.testing.allocator;
    var decoder = Self.init(alloc);

    try decoder.load("./tests/many_register_mov");
    defer decoder.free();

    const res = try decoder.decode();
    defer alloc.free(res);

    const expected =
        \\bits 16
        \\
        \\mov cx, bx
        \\mov ch, ah
        \\mov dx, bx
        \\mov si, bx
        \\mov bx, di
        \\mov al, cl
        \\mov ch, ch
        \\mov bx, ax
        \\mov bx, si
        \\mov sp, di
        \\mov bp, ax
    ;

    try testing.expectEqualDeep(expected, res);
}

test "load_immediat" {
    const alloc = std.testing.allocator;
    var decoder = Self.init(alloc);

    try decoder.load("./tests/load_immediat");
    defer decoder.free();

    const res = try decoder.decode();
    defer alloc.free(res);

    const expected =
        \\bits 16
        \\
        \\mov ax, 300
        \\mov bl, 22
    ;

    try testing.expectEqualDeep(expected, res);
}
