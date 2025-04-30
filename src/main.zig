const std = @import("std");
const Decoder = @import("decoder.zig");
const DecoderErr = @import("decoder.zig");

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const alloc = gpa.allocator();

    var decoder = Decoder.init(alloc);

    try decoder.load("./tests/load_immediat");
    defer decoder.free();

    const res = try decoder.decode();
    defer alloc.free(res);

    std.debug.print("Result:\n{s}\n", .{res});
}
