#!/usr/bin/env python3

w_r = 0.299
w_b = 0.114
w_g = 1 - w_r - w_b

u_max = 0.436
v_max = 0.615


def yuv2rgb(y, u, v):
    return (
        y + v * (1 - w_r) / v_max,
        y - u * w_b * (1 - w_b) / (u_max * w_g) - v * w_r * (1 - w_r) / (v_max * w_g),
        y + u * (1 - w_b) / u_max
        # y + 1.13983 * (v - 128),
        # y - 0.39465 * (u - 128) - 0.58060 * (v - 128),
        # y + 2.03211 * (u - 128)
    )


def rgb2yuv(r, g, b):
    y = w_r * r + w_g * g + w_b * b
    return (
        y,
        u_max * ((b - y) / (1 - w_b)),
        v_max * ((r - y) / (1 - w_r))
        # -0.14713 * r - 0.28886 * g + 0.436 * b + 128,
        # 0.615 * r - 0.51499 * g - 0.10001 * b + 128
    )


if __name__ == '__main__':
    a = rgb2yuv(0, 0, 1)
    b = rgb2yuv(0, 1, 0)
    print(yuv2rgb(*a))
    print(yuv2rgb(*b))
    c = a[0], b[1], b[2]
    print(c)
    print(yuv2rgb(*c))


