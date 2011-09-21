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
            this.labDatapath.Text = Program.dp.datapath;
        }


        private void button1_Click(object sender, EventArgs e)
        {
        }

        private void button2_Click(object sender, EventArgs e)
        {
            ParticletypeList f = new ParticletypeList();
            f.Show();
        }

        private void button5_Click(object sender, EventArgs e)
        {
            ParticleGeneratorList f = new ParticleGeneratorList();
            f.Show();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            UnitclassList f = new UnitclassList();
            f.Show();
        }

        private void button6_Click(object sender, EventArgs e)
        {
            ModifierList f = new ModifierList();
            f.Show();
        }

        private void button4_Click(object sender, EventArgs e)
        {
            WeapontypeList f = new WeapontypeList();
            f.Show();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Program.dp.save(Program.dp.datapath);
        }
    }
}
