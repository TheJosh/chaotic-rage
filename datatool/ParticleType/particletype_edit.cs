using System;
using System.Collections.Generic;
using System.ComponentModel;

using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class ParticletypeEdit : Form
    {
        private particletype_item item;

        public particletype_item Item
        {
            get { return this.item; }
        }

        public ParticletypeEdit(particletype_item item)
        {
            InitializeComponent();
            this.item = item;
        }

        private void AreatypeEdit_Load(object sender, EventArgs e)
        {
            this.Text = this.item.getName();
            this.grid.SelectedObject = this.item;
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        private void btnActions_Click(object sender, EventArgs e)
        {
        }
    }
}
