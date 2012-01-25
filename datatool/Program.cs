using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;

namespace datatool
{
    static class Program
    {
        // Currently loaded module
        public static DataProvider dp = null;

        // Directory containing game mods
        public static string game_path;

        // Directory containing user mods
        public static string user_path;



        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            // The game mods are in a sub-dir of the current directory,
            // unless if we are running out of the Debug directory for the datatool.
            // In that case, we assume a chaotic rage install is a few directories above.
            game_path = Path.GetDirectoryName(Application.ExecutablePath);
            game_path = game_path.Replace("\\datatool\\bin\\Debug", "");
            game_path += "\\data";

            // The user mods are in "Application Data" on windows.
            // This tool doesn't really run on any other platform so other systems don't
            // matter at this time.
            user_path = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
            user_path += "\\Chaotic Rage\\mods";
            Directory.CreateDirectory(user_path);


            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Main());
        }
    }
}
