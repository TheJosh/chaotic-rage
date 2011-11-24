using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
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
        }

        private void Main_Load(object sender, EventArgs e)
        {
            this.statusDatapath.Text = Program.dp.datapath;
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Program.dp.save(Program.dp.datapath);
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
    }
}
