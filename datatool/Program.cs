using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace datatool
{
    static class Program
    {
        public static string datapath;

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            // The datapath is the current directory,
            // unless if we are running out of the Debug directory for the datatool.
            // In that case, we assume a chaotic rage install is a couple directories above.
            datapath = System.IO.Path.GetDirectoryName(Application.ExecutablePath);
            datapath = datapath.Replace("\\datatool\\bin\\Debug", "");

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Main());
        }
    }
}
