using System.Runtime.InteropServices;

namespace FotiadiMath
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct DataForMetricConvolutionWithShift
    {
        /// <summary>Размер окна, в котором проверяется похожесть отрезков трасс. В идеале, должен быть равен длине волны.</summary>
        public ushort r_idx;
        public double k_min_count;
        public double shift_signal_x;
        public double shift_signal_y;
    }
}
