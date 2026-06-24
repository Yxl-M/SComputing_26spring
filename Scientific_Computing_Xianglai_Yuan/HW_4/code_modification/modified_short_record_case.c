/*
 * Modified short_record case for HomeWork 04, Part 7 Option B
 * 作业 04 第 7 部分 Option B 的 short_record 修改片段
 *
 * English:
 * The original short_record case used only 64 samples at fs = 512 Hz.
 * That gave T = 64 / 512 = 0.125 s and Delta f = 8 Hz.
 * The modified case uses 512 samples at the same fs = 512 Hz.
 * That gives T = 512 / 512 = 1.0 s and Delta f = 1 Hz.
 * This makes the 50 Hz and 55 Hz peaks easier to distinguish.
 *
 * 中文：
 * 原来的 short_record 情况在 fs = 512 Hz 时只使用 64 个样本，
 * 所以 T = 64 / 512 = 0.125 s，频率分辨率 Delta f = 8 Hz。
 * 修改后保持 fs = 512 Hz 不变，把样本数增加到 512，
 * 所以 T = 512 / 512 = 1.0 s，频率分辨率 Delta f = 1 Hz。
 * 这样 50 Hz 和 55 Hz 两个峰会更容易区分。
 */

{
    "short_record",
    "The signal contains 50 Hz and 55 Hz. This modified case uses a longer record, so the FFT can separate the nearby frequencies more clearly.",
    close_frequencies_signal,
    512.0,
    512U,
},
