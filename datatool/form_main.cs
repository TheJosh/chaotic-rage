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
            this.labDatapath.Text = Program.datapath;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            AreatypeList f = new AreatypeList();
            f.Show();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            ParticletypeList f = new ParticletypeList();
            f.Show();
        }
    }
}
