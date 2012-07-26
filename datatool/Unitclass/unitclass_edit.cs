using System;
using System.Collections.Generic;
using System.ComponentModel;

using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class UnitclassEdit : Form
    {
        private unitclass_item item;
        private ExtraListItem sel_list_item;

        public unitclass_item Item
        {
            get { return this.item; }
        }

        public UnitclassEdit(unitclass_item item)
        {
            InitializeComponent();
            this.item = item;
        }


        /**
         * Form load
         **/
        private void Form_Load(object sender, EventArgs e)
        {
            this.Text = this.item.getName();

            grid.SelectedObject = this.item;
        }

        /**
         * Save
         **/
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


        
    }

}
