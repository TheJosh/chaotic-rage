using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace Maptool
{
    static class Program
    {
        static public string Datapath;

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Datapath = System.IO.Path.GetDirectoryName(Application.ExecutablePath);
            Datapath = Datapath.Replace("\\maptool\\bin\\Debug", "");
            Datapath += "\\data\\cr";

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new frmMain());
        }
    }
}
