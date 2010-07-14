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

        public areatype_item Item
        {
            get { return this.item; }
        }

        public AreatypeEdit(areatype_item item)
        {
            InitializeComponent();
            this.item = item;
        }

        private void AreatypeEdit_Load(object sender, EventArgs e)
        {
            this.Text = this.item.getName();
            this.grid.SelectedObject = this.item;
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            
        }
    }
}
