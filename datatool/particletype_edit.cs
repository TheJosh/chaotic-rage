using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class ParticletypeEdit : Form
    {
        private particletype_item item;
        private particletype_item internal_item;

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

            this.internal_item = this.item.clone();
            this.grid.SelectedObject = this.internal_item;
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            this.item = this.internal_item;
        }
    }
}
