using FotiadiMath;

namespace ConsoleApp1
{
    internal class AutoCorrelationTests
    {
        public static void ExampleWithSameTracesWithNoBorders()
        {
            ushort r_idx = 10;
            short max_shift_point_idx = 5;
            short[] x2y = new short[TestData.firstTraceSignals.Length];
            short[] y2x = new short[TestData.firstTraceSignals.Length];
            var status = FotiadiMathWrapper.Correlate2Traces(
                TestData.firstTraceSignals,
                TestData.firstTraceSignals,
                max_shift_point_idx,
                0,
                null,
                null,
                r_idx,
                x2y,
                y2x,
                out var target_func
            );

            if (status == 0)
                Console.WriteLine("First one example is completed successfully.");
        }

        public static void ExampleWithDifferentTracesWithBorders()
        {
            ushort r_idx = 10;
            short max_shift_point_idx = 5;
            short[] x2y = new short[TestData.firstTraceSignals.Length];
            short[] y2x = new short[TestData.secondTraceSignals.Length];

            const short numOfBorders = 2;
            // пояснение:
            // первая граница должна отображаться из индекса 0 на первой трассе в индекс 0 во второй трассе и обратно (такое ограничение нет смысла задавать, т.к. оно и так задано по умолчанию, но задать его - не ошибка)
            // вторая граница должна отображаться из индекса 54 на первой трассе в индекс 51 во второй трассе и обратно
            short[] bordersXTrace = new short[numOfBorders] { 0, 54 };
            short[] bordersYTrace = new short[numOfBorders] { 0, 51 };

            var status = FotiadiMathWrapper.Correlate2Traces(
                TestData.firstTraceSignals,
                TestData.secondTraceSignals,
                max_shift_point_idx,
                numOfBorders, // две границы
                bordersXTrace,
                bordersYTrace,
                r_idx,
                x2y,
                y2x,
                out var target_func
            );

            if (status == 0)
                Console.WriteLine("Second one example is completed successfully.");
        }

        /// <summary>
        ///   <para>Границы заданы некорректно, решение не найдено.</para>
        ///   <para>Значение -32768 в x2y или y2x означают, что не было найдено отображение.</para>
        /// </summary>
        public static void ExampleWithDifferentTracesWithIncorrectBorders()
        {
            ushort r_idx = 10;
            short max_shift_point_idx = 5;
            short[] x2y = new short[TestData.firstTraceSignals.Length];
            short[] y2x = new short[TestData.secondTraceSignals.Length];

            const short numOfBorders = 2;
            // пояснение:
            // первая граница задана некорректно - нельзя отобразить 0 в 51, так как точка (0;0) зафиксирована по умолчанию
            // вторая граница задана некорректо - нельзя отобразить 55 в 3, так как точка (55;52) зафиксирована по умолчанию
            short[] bordersXTrace = new short[numOfBorders] { 0, 55 };
            short[] bordersYTrace = new short[numOfBorders] { 51, 3 };

            var status = FotiadiMathWrapper.Correlate2Traces(
                TestData.firstTraceSignals,
                TestData.secondTraceSignals,
                max_shift_point_idx,
                numOfBorders, // две границы
                bordersXTrace,
                bordersYTrace,
                r_idx,
                x2y,
                y2x,
                out var target_func
            );

            if (status == 0)
                Console.WriteLine("Third one example is completed successfully.");
        }

        /// <summary>Пример со случайно, но корректно заданными фиксированными границами.</summary>
        public static void ExampleWithDifferentTracesWithStrangeBorders()
        {
            ushort r_idx = 10;
            short max_shift_point_idx = 20;
            short[] x2y = new short[TestData.firstTraceSignals.Length];
            short[] y2x = new short[TestData.secondTraceSignals.Length];

            const short numOfBorders = 3;
            // пояснение:
            // размер диагонали в матрице корреляции = 20 = max_shift_point_idx
            // значит границу можно фиксировать на расстоянии x +- 10
            // из 1 в 10 - корректное отображение
            short[] bordersXTrace = new short[numOfBorders] { 1, 2, 50 };
            short[] bordersYTrace = new short[numOfBorders] { 10, 11, 50 };

            var status = FotiadiMathWrapper.Correlate2Traces(
                TestData.firstTraceSignals,
                TestData.secondTraceSignals,
                max_shift_point_idx,
                numOfBorders,
                bordersXTrace,
                bordersYTrace,
                r_idx,
                x2y,
                y2x,
                out var target_func
            );

            if (status == 0)
                Console.WriteLine("Fourth one example is completed successfully.");
        }
    }
}
