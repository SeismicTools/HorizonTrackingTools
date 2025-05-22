using System;
using System.Runtime.InteropServices;

namespace FotiadiMath
{
    public sealed class FotiadiMathWrapper
    {
        /// <summary>
        ///   <para>Корреляция пары трасс (ф-ия автокорреляции). Возможны разные длины сигналов и задание фиксированных точек (горизонтов).</para>
        ///   <para>Значение -32768 в x2y или y2x означают, что не было найдено отображение.<br /></para>
        ///   <para>Из-за особенностей реализации, связанной с корреляционным планшетом, точки (0;0) и (last_id_x, last_id_y) считаются зафиксированными. Из-за этого не получится задать фиксацию для этих индексов. P.S. Такое ограничение возникло в связи с договоренностью, что данный алгоритм будет запускаться на сейсмических сигналах меж двух, зафиксированных границ.</para>
        ///   <para>Также, если задана точка фиксации, выходящая за диагональ, то она не будет учтена, т.к. при её учете не существовало бы решений.</para>
        ///   <para>Фиксированные границы не обязательно будут проходить в заданном индексе. Из-за существующей возможности движения по матрице корреляции по диагоналям, фиксированные точки могут колебаться в +- 1 индекс. Если хочется строгой фиксации, нужна доработка.</para>
        /// </summary>
        /// <param name="signal_x">Сигналы первой трассы.</param>
        /// <param name="signal_y">Сигналы второй трассы.</param>
        /// <param name="max_shift_point_idx">Максимальный сдвиг каждой точки по индексам - другими словами, ширина диагонали в корреляционной матрице.</param>
        /// <param name="n_fix_borders">Количество зафиксированных границ.</param>
        /// <param name="idsOfFixedPointsAtXTrace">Индексы зафиксированных точек на трассе X.</param>
        /// <param name="idsOfFixedPointsAtYTrace">Индексы зафиксированных точек на трассе Y.</param>
        /// <param name="r_idx">Размер окна, в котором проверяется похожесть отрезков трасс. В идеале, должен быть равен длине волны.</param>
        /// <param name="x2y">Отображения индексов первой трассы на индексы второй.</param>
        /// <param name="y2x">Отображения индексов второй трассы на индексы первой.</param>
        /// <param name="target_function">Метрика похожести пары трасс - меньше = лучше.</param>
        /// <returns>Код ошибки. 0 - успех.<br /></returns>
        public static int Correlate2Traces(
            double[] signal_x,
            double[] signal_y,
            short max_shift_point_idx,
            short n_fix_borders,
            short[] idsOfFixedPointsAtXTrace,
            short[] idsOfFixedPointsAtYTrace,
            ushort r_idx,
            short[] x2y,
            short[] y2x,
            out double target_function
        )
        {
            var dataForMetricFunc = new DataForMetricConvolutionWithShift
            {
                r_idx = r_idx,
                // значения ниже в стандартной функции метрики не применяются
                k_min_count = 2.0 / 3.0,
                shift_signal_x = 0,
                shift_signal_y = 0
            };

            int statusCode = 1;

            // Аллоцируем память для структуры
            int size = Marshal.SizeOf(dataForMetricFunc);
            IntPtr pdata_fm = Marshal.AllocHGlobal(size);
            try
            {
                // Копируем структуру в выделенную память
                Marshal.StructureToPtr(dataForMetricFunc, pdata_fm, true);

                statusCode = FM_CorrelationByWaveAlgorithmFor2TracesWithDefaultMetricFunction(
                    (short)signal_x.Length,
                    (short)signal_y.Length,
                    signal_x,
                    signal_y,
                    max_shift_point_idx,
                    n_fix_borders,
                    idsOfFixedPointsAtXTrace,
                    idsOfFixedPointsAtYTrace,
                    pdata_fm,
                    x2y,
                    y2x,
                    out target_function
                );
            }
            finally
            {
                // Освобождаем выделенную память
                Marshal.FreeHGlobal(pdata_fm);
            }

            return statusCode;
        }

        /// <summary>
        ///   <para>
        /// Подход корреляции отражающих горизонтов, основанный на корреляции соседей.<br />Границы имеют номера, кратные 10. Сначала 10, потом -20, 30, -40 и т.д. Такая нумерация была выбранна для корректного отображения в Desmana.
        /// </para>
        ///   <para>Значения границ, равные -2147483648, означают пустые места, куда граница не распространилась.</para>
        /// </summary>
        /// <param name="inlineCount">Количество трасс вдоль оси X.</param>
        /// <param name="crosslineCount">Количество трасс вдоль оси Y.</param>
        /// <param name="numOfSignalsAtOneTrace">Число сигналов в одной трассе.</param>
        /// <param name="signals">Сигналы всех трасс (длина выровнена по numOfSignalsAtOneTrace).</param>
        /// <param name="max_shift_point_idx">Максимальный сдвиг каждой точки по индексам - другими словами, ширина диагонали в корреляционной матрице.</param>
        /// <param name="countOfFixedBorders">Число зафиксированных границ.</param>
        /// <param name="surface_idx">Зафиксированные границы (одна граница = inlineCount * crosslineCount чисел, обозначающих номер индекса в трассе).</param>
        /// <param name="callbackData">Пользовательский объект для отслеживания прогресса.</param>
        /// <param name="callback">Функция для изменения значения прогресса.</param>
        /// <param name="r_idx">Размер окна, в котором проверяется похожесть пары сигналов.</param>
        /// <param name="inlineIdRef">Опорная трасса (координата по x), с которой начнется обход остовного дерева при прослеживании границ.</param>
        /// <param name="crosslineIdRef">Опорная трасса (координата по y), с которой начнется обход остовного дерева при прослеживании границ.</param>
        /// <param name="spaceBetweenTwoHorizons">Расстояние в индексах между двумя соседними границами (на опорной трассе).</param>
        /// <param name="horizonsBreadth">Размер каждой границы.</param>
        /// <returns>Прослеженные номера границ. Один номер - одна поверхность границы.</returns>
        public static int[] DefineReflectingHorizons_neighbourVariant(
           int inlineCount,
           int crosslineCount,
           int numOfSignalsAtOneTrace,
           double[] signals,
           int max_shift_point_idx,
           int countOfFixedBorders,
           short[] surface_idx,
           object callbackData,
           SetProgressValueFunc callback,
           int r_idx,
           int inlineIdRef,
           int crosslineIdRef,
           int spaceBetweenTwoHorizons,
           int horizonsBreadth
        )
        {
            // Аллоцируем память для класса
            GCHandle handle = GCHandle.Alloc(callbackData, GCHandleType.Normal);

            int result = -1;
            int[] temp;

            try
            {
                var callbackDataRaw = GCHandle.ToIntPtr(handle);

                result = FM_Seismic3DCorrelation(
                    inlineCount,
                    crosslineCount,
                    numOfSignalsAtOneTrace,
                    signals,
                    max_shift_point_idx,
                    countOfFixedBorders,
                    surface_idx,
                    Array.Empty<short>(),
                    callbackDataRaw,
                    callback,
                    r_idx,
                    out var out_correlationData
                );

                short minIdRefTrace = 0;
                short maxIdRefTrace = (short)(numOfSignalsAtOneTrace - 1);
                if(countOfFixedBorders >= 2)
                {
                    minIdRefTrace = surface_idx[inlineIdRef + inlineCount * crosslineIdRef];
                    maxIdRefTrace = surface_idx[inlineIdRef + inlineCount * crosslineIdRef + inlineCount * crosslineCount * (countOfFixedBorders - 1)];
                }
                short[] surface_idx_refTrace = { minIdRefTrace, maxIdRefTrace };

                result = FM_S3DDefineReflectingHorizonsForEntireTraceNew(
                    out_correlationData,
                    inlineIdRef,
                    crosslineIdRef,
                    2,
                    surface_idx_refTrace,
                    spaceBetweenTwoHorizons,
                    horizonsBreadth,
                    callbackDataRaw,
                    callback,
                    horizonsBreadth,
                    out var out_surfaceIds
                );

                var totalLength = inlineCount * crosslineCount * numOfSignalsAtOneTrace;
                var managedArray = new int[totalLength];
                Marshal.Copy(out_surfaceIds, managedArray, 0, totalLength);

                FM_S3D_Free(out_correlationData);
                FM_S3D_FreeArray(out_surfaceIds);

                temp = managedArray;
            }
            finally
            {
                if(handle.IsAllocated)
                    handle.Free();
            }

            return temp;
        }

        /// <summary>
        ///   <para>
        /// Подход корреляции отражающих горизонтов, основанный на корреляции соседей.<br />Границы имеют номера, кратные 10. Сначала 10, потом -20, 30, -40 и т.д. Такая нумерация была выбрана для корректного отображения в Desmana.
        /// </para>
        ///   <para>Значения границ, равные -2147483648, означают пустые места, куда граница не распространилась.<br /></para>
        /// </summary>
        /// <param name="inlineCount">Количество трасс вдоль оси X.</param>
        /// <param name="crosslineCount">Количество трасс вдоль оси Y.</param>
        /// <param name="numOfSignalsAtOneTrace">Число сигналов в одной трассе.</param>
        /// <param name="signals">Сигналы всех трасс (длина выровнена по numOfSignalsAtOneTrace).</param>
        /// <param name="max_shift_point_idx">Максимальный сдвиг каждой точки по индексам - другими словами, ширина диагонали в корреляционной матрице.</param>
        /// <param name="countOfFixedBorders">Число зафиксированных границ.</param>
        /// <param name="surface_idx">Зафиксированные границы (одна граница = inlineCount * crosslineCount чисел, обозначающих номер индекса в трассе).</param>
        /// <param name="callbackData">Объект для отслеживания прогресса.</param>
        /// <param name="callback">Функция для изменения значения прогресса.</param>
        /// <param name="r_idx">Размер окна, в котором проверяется похожесть пары сигналов.</param>
        /// <param name="inlineIdRef">Опорная трасса (координата по x), с которой начнется обход остовного дерева при прослеживании границ, а также с которой будут коррелироваться остальные трассы.</param>
        /// <param name="crosslineIdRef">Опорная трасса (координата по y), с которой начнется обход остовного дерева при прослеживании границ, а также с которой будут коррелироваться остальные трассы.</param>
        /// <param name="spaceBetweenTwoHorizons">Расстояние в индексах между двумя соседними границами (на опорной трассе).</param>
        /// <param name="horizonsBreadth">Размер каждой границы.</param>
        /// <returns>Прослеженные номера границ. Один номер - одна поверхность границы.<br /></returns>
        public static int[] DefineReflectingHorizons_RefTraceVariant(
           int inlineCount,
           int crosslineCount,
           int numOfSignalsAtOneTrace,
           double[] signals,
           int max_shift_point_idx,
           int countOfFixedBorders,
           short[] surface_idx,
           object callbackData,
           SetProgressValueFunc callback,
           int r_idx,
           int inlineIdRef,
           int crosslineIdRef,
           int spaceBetweenTwoHorizons,
           int horizonsBreadth
        )
        {
            GCHandle handle = GCHandle.Alloc(callbackData, GCHandleType.Normal);

            int[] temp;

            try
            {
                var callbackDataRaw = GCHandle.ToIntPtr(handle);

                FM_Seismic3DCorrelation_one_to_many(
                    inlineCount: inlineCount,
                    crosslineCount: crosslineCount,
                    numOfSignalsAtOneTrace: numOfSignalsAtOneTrace,
                    signals: signals,
                    max_shift_point_idx: max_shift_point_idx,
                    countOfFixedBorders: countOfFixedBorders,
                    surface_idx: surface_idx,
                    Array.Empty<short>(),
                    callbackData: callbackDataRaw,
                    callback,
                    r_idx: r_idx,
                    mainTraceCoordByX: inlineIdRef,
                    mainTraceCoordByY: crosslineIdRef,
                    horizonsBreadth,
                    spaceBetweenTwoHorizons,
                    out var out_corr_data
                );

                var totalLength = inlineCount * crosslineCount * numOfSignalsAtOneTrace;
                var managedArray = new int[totalLength];
                Marshal.Copy(out_corr_data, managedArray, 0, totalLength);
                temp = managedArray;

                FM_S3D_FreeArray(out_corr_data);
            }
            finally
            {
                if(handle.IsAllocated)
                    handle.Free();
            }

            return temp;
        }

        /// <summary>Функция для выставления прогресса.</summary>
        /// <param name="value">Новое значение прогресса.</param>
        /// <param name="data">В cpp это const void*. Сюда будет передаваться ваш объект обратно, чтобы производить с ним какие-либо действия.</param>
        /// <returns>
        ///   <br />
        /// </returns>
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int SetProgressValueFunc( double value, IntPtr data );

        [DllImport("FotiadiMath.win.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int FM_CorrelationByWaveAlgorithmFor2TracesWithDefaultMetricFunction(
            short n_grid_x,
            short n_grid_y,
            double[] signal_x,
            double[] signal_y,
            short max_shift_point_idx,
            short n_fix_borders,
            short[] idsOfFixedPointsAtXTrace,
            short[] idsOfFixedPointsAtYTrace,
            IntPtr pdata_fm,
            short[] x2y,
            short[] y2x,
            out double target_function
        );

        [DllImport("FotiadiMath.win.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int FM_Seismic3DCorrelation(
           int inlineCount,
           int crosslineCount,
           int numOfSignalsAtOneTrace,
           double[] signals,
           int max_shift_point_idx,
           int countOfFixedBorders,
           short[] surface_idx,
           short[] fix_point_idx_r,
           IntPtr callbackData,
           SetProgressValueFunc callback,
           int r_idx,
           out IntPtr out_correlationData
        );

        [DllImport("FotiadiMath.win.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int FM_S3D_Free( IntPtr savedObject );

        [DllImport("FotiadiMath.win.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int FM_S3D_FreeArray( IntPtr savedObjectWithCorrelationData );

        [DllImport("FotiadiMath.win.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int FM_S3DDefineReflectingHorizonsForEntireTraceNew(
            IntPtr savedObjectWithCorrelationData,
            int inlineId,
            int crosslineId,
            int countOfFixedBorders,
            [In, Out] short[] surface_idx,
            int spacesBetweenTwoHorizons,
            int sizeOfOneHorizon,
            IntPtr callbackData,
            SetProgressValueFunc callback,
            int horizonsBreadth,
            out IntPtr out_surfaceIds
        );

        [DllImport("FotiadiMath.win.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int FM_Seismic3DCorrelation_one_to_many(
            int inlineCount,
            int crosslineCount,
            int numOfSignalsAtOneTrace,
            [In] double[] signals,
            int max_shift_point_idx,
            int countOfFixedBorders,
            [In, Out] short[] surface_idx,
            [In] short[] fix_point_idx_r,
            IntPtr callbackData,
            SetProgressValueFunc callback,
            int r_idx,
            int mainTraceCoordByX,
            int mainTraceCoordByY,
            int horizonsBreadth,
            int spaceBetweenHorizons,
            out IntPtr out_correlationData
        );
    }
}
