using System;
using System.Collections.Generic;
using System.ComponentModel;
using Ionic.Utils.Zip;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace datatool
{
    public partial class Main : Form
    {
        public Main()
        {
            InitializeComponent();
            this.listeners = new List<ListenForm>();
        }


        private List<ListenForm> listeners;

        public void registerListener(ListenForm f)
        {
            this.listeners.Add(f);
        }

        public void unregisterListener(ListenForm f)
        {
            this.listeners.Remove(f);
        }

        public void postMessage_Updated()
        {
            foreach (ListenForm l in this.listeners)
            {
                l.Message_Updated();
            }
        }


        private void Main_Load(object sender, EventArgs e)
        {
            this.UpdateFileStatus();
            this.ScanDirectories();
        }

        private void Main_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (Program.dp == null) return;
            switch (MessageBox.Show("Do you want to save?", Application.ProductName, MessageBoxButtons.YesNoCancel))
            {
                case System.Windows.Forms.DialogResult.Yes:
                    Program.dp.save();
                    break;

                case System.Windows.Forms.DialogResult.Cancel:
                    e.Cancel = true;
                    break;
            }
        }


        /**
         * Update the status bar, menus, etc after file load/save/close
         */
        private void UpdateFileStatus()
        {
            bool enabled = true;

            if (Program.dp == null) {
                this.statusDatapath.Text = "No file loaded";
                enabled = false;
            } else {
                this.statusDatapath.Text = "Current File: " + Program.dp.GetFriendlyFilename();
            }

            metricsToolStripMenuItem.Enabled = enabled;
            saveToolStripMenuItem.Enabled = enabled;
            toolbar.Enabled = enabled;
        }


        /**
         * Scan directories for files to show in the menu
         **/
        private void ScanDirectories()
        {
            string[] files;
            bool found = false;

            // Local crks
            files = Directory.GetFiles(Program.user_path, "*.crk");
            Array.Reverse(files);
            foreach (string f in files) {
                ToolStripMenuItem i = new ToolStripMenuItem();
                i.Text = "Open '" + Path.GetFileName(f) + "'";
                i.Tag = new Main.CustomModInfo(f, true);
                i.Click += CustomMod_Click;
                fileToolStripMenuItem.DropDown.Items.Insert(2, i);
                found = true;
            }

            // Directories in the game
            // Shipped (final) version will have these as .crk's, so they won't show up.
            files = Directory.GetDirectories(Program.game_path);
            Array.Reverse(files);
            foreach (string f in files) {
                ToolStripMenuItem i = new ToolStripMenuItem();
                i.Text = "Open '" + Path.GetFileName(f) + "'";
                i.Tag = new Main.CustomModInfo(f, false);
                i.Click += CustomMod_Click;
                fileToolStripMenuItem.DropDown.Items.Insert(2, i);
                found = true;
            }

            if (found) {
                fileToolStripMenuItem.DropDown.Items.Insert(2, new ToolStripSeparator());
            }
        }

        /**
         * Saves some stuff for the tag + event handler
         **/
        private class CustomModInfo
        {
            public string filename;
            public bool zip;
            public CustomModInfo(string f, bool z) { this.filename = f; this.zip = z; }
        }

        /**
         * Handles events on module clicks
         **/
        private void CustomMod_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem i = (ToolStripMenuItem) sender;
            Main.CustomModInfo cmi = (Main.CustomModInfo)i.Tag;

            Program.dp = new DataProvider();
            //try {
                Program.dp.load(cmi.filename, cmi.zip);
           // } catch (Exception ex) {
            //    MessageBox.Show("Tried to load mod, but failed. It might be corrupt.\n\nError:\n" + ex.Message);
            //    Program.dp = null;
            //}
            UpdateFileStatus();
        }

        /**
         * Open a file
         **/
        private void openModToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog d = new OpenFileDialog();
            d.Filter = "Chaotic Rage Datafiles (*.crk)|*.crk|All Files (*.*)|*.*";
            if (d.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
                Program.dp = new DataProvider();
                try {
                    Program.dp.load(d.FileName, true);
                } catch (Exception ex) {
                    MessageBox.Show("Tried to load mod, but failed. It might be corrupt.\n\nError:\n" + ex.Message);
                    Program.dp = null;
                }
                UpdateFileStatus();
            }
        }

        /**
         * Create a new file
         **/
        private void newModToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog d = new SaveFileDialog();
            d.Filter = "Chaotic Rage Datafiles (*.crk)|*.crk|All Files (*.*)|*.*";
            d.InitialDirectory = Program.user_path;
            if (d.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
                Program.dp = new DataProvider();
                Program.dp.SetFile(d.FileName, true);
                UpdateFileStatus();
            }
        }



        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Program.dp.save();
        }

        private void toolWeapons_Click(object sender, EventArgs e)
        {
            WeapontypeList f = new WeapontypeList();
            f.MdiParent = this;
            f.Show();
        }

        private void toolUnittypes_Click(object sender, EventArgs e)
        {
            UnitclassList f = new UnitclassList();
            f.MdiParent = this;
            f.Show();
        }


        private void MenuMetricsWeapons_Click(object sender, EventArgs e)
        {
            metric_grid f = new metric_grid(new weapontype_item("A").getMetricNames(), Program.dp.WeaponTypes);
            f.MdiParent = this;
            f.Show();
        }

        private void MenuMetricsWeaponsCompare_Click(object sender, EventArgs e)
        {
            metric_compare f = new metric_compare(new weapontype_item("A").getMetricNames(), Program.dp.WeaponTypes);
            f.MdiParent = this;
            f.Show();
        }

        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            GeneralEdit edit = new GeneralEdit(Program.dp.UnitClasses, "Unit Types", typeof(unitclass_item));
            edit.Show();
        }

        private void toolAnimModels_Click(object sender, EventArgs e)
        {
            GeneralEdit edit = new GeneralEdit(Program.dp.AnimModels, "Animation Models", typeof(animmodel_item), new AnimmodelPreview());
            edit.Show();
        }

    }
}
