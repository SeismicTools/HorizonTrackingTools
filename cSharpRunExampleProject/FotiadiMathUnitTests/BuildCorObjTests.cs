using FotiadiMath;

namespace ConsoleApp1
{
    internal class BuildCorObjTests
    {
        public static void Test()
        {
            // заданы 2 границы (первая на всех 3-х индексах, вторая на всех X индексах)
            short lowBorder = (short)(TestData.firstTraceSignals.Length-5);
            short[] surfaceIdx = [3, 3, 3, 3, 3, 3, 3, 3, 3, lowBorder, lowBorder, lowBorder, lowBorder, lowBorder, lowBorder, lowBorder, lowBorder, lowBorder];

            var callbackData = new TestProgressReporter();

            var horizonsArray = FotiadiMathWrapper.DefineReflectingHorizons_neighbourVariant(
                inlineCount: 3,
                crosslineCount: 3,
                numOfSignalsAtOneTrace: TestData.firstTraceSignals.Length,
                signals: TestData.ExpandArrayCyclic(TestData.firstTraceSignals, 9),
                max_shift_point_idx: 10,
                countOfFixedBorders: 2,
                surfaceIdx,
                callbackData,
                TestProgressReporter.ReportProgress,
                r_idx: 10,
                inlineIdRef: 0,
                crosslineIdRef: 0,
                spaceBetweenTwoHorizons: 3,
                horizonsBreadth: 3
            );

            for(int i = 0; i < horizonsArray.Length; i++)
            {
                Console.WriteLine(horizonsArray[i]);
            }
        }

        public static void Test2()
        {
            short lowBorder = (short)(TestData.firstTraceSignals.Length-5);
            short[] surfaceIdx = [3, 3, 3, 3, 3, 3, 3, 3, 3, lowBorder, lowBorder, lowBorder, lowBorder, lowBorder, lowBorder, lowBorder, lowBorder, lowBorder];

            var callbackData = new TestProgressReporter();

            var horizonsArray = FotiadiMathWrapper.DefineReflectingHorizons_RefTraceVariant(
                inlineCount: 3,
                crosslineCount: 3,
                numOfSignalsAtOneTrace: TestData.firstTraceSignals.Length,
                signals: TestData.ExpandArrayCyclic(TestData.firstTraceSignals, 9),
                max_shift_point_idx: 10,
                countOfFixedBorders: 2,
                surfaceIdx,
                callbackData,
                TestProgressReporter.ReportProgress,
                r_idx: 10,
                inlineIdRef: 0,
                crosslineIdRef: 0,
                spaceBetweenTwoHorizons: 3,
                horizonsBreadth: 3
            );

            for(int i = 0; i < horizonsArray.Length; i++)
            {
                Console.WriteLine(horizonsArray[i]);
            }
        }
    }
}
