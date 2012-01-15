using System;
using System.Collections.Generic;
using System.ComponentModel;
using Ionic.Utils.Zip;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

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
            this.statusDatapath.Text = "Current File: " + Program.dp.GetFriendlyFilename();
        }

        private void Main_FormClosing(object sender, FormClosingEventArgs e)
        {
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



        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Program.dp.save();
        }

        private void toolParticles_Click(object sender, EventArgs e)
        {
            ParticletypeList f = new ParticletypeList();
            f.MdiParent = this;
            f.Show();
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

    }
}
