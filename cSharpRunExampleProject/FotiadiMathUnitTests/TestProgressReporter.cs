using System.Runtime.InteropServices;

namespace ConsoleApp1
{
    internal class TestProgressReporter
    {
        // значение прогресса
        public int Progress { get; private set; } = 0;

        // диапазон значений прогресса
        public int MinProgress { get; private set; } = 0;

        public int MaxProgress { get; private set; } = 100;

        // установить пояснительную подпись для прогресса
        public string ProgressText { get; private set; } = "";

        public void SetProgressValue(int value)
        {
            Progress = value;
        }

        public void SetProgressRange(int min, int max)
        {
            MinProgress = min;
            MaxProgress = max;
        }

        public void SetProgressText(string text)
        {
            ProgressText = text;
        }

        public static int ReportProgress(double value, IntPtr data)
        {
            GCHandle handle = GCHandle.FromIntPtr(data);
            object callbackData = handle.Target; // Получаем объект
            var progressReporter = (TestProgressReporter)callbackData;
            int progress = (int)(value * 1000);
            progressReporter.SetProgressValue(progress);
            int isProcessCancelled = 0; // false
            return isProcessCancelled;
        }
    }
}
