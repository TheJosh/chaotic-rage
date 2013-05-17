using System;
using System.Collections.Generic;
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
            Datapath += System.IO.Path.DirectorySeparatorChar + "data" + System.IO.Path.DirectorySeparatorChar +  "cr";

            if (! System.IO.Directory.Exists(Datapath)) {
                MessageBox.Show("Invalid data directory '" + Datapath + "'");
                return;
            }

            Tools.Init();

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new frmMain());
        }
    }
}
