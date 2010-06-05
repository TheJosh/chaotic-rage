using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class AreatypeEdit : Form
    {
        private areatype_item item;

        public AreatypeEdit(areatype_item item)
        {
            InitializeComponent();
            this.item = item;
        }

        private void AreatypeEdit_Load(object sender, EventArgs e)
        {
            this.Text = item.getName();
            this.grid.SelectedObject = item;
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            // todo: logic to save
        }
    }
}
