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
        private ExtraListItem sel_list_item;

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
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void soundToolStripMenuItem_Click(object sender, EventArgs e)
        {
            sound sound = new sound();

            this.item.Sounds.Add(sound);

            ExtraListItem item = new ExtraListItem();
            item.Extra = sound;
            item.Group = lstSections.Groups[0];
            this.lstSections.Items.Add(item);
        }

        private void toolStripButton2_Click(object sender, EventArgs e)
        {
            lstSections.SelectedItems.Clear();
            grid.SelectedObject = item;
        }

        private void lstSections_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (this.sel_list_item != null)
            {
                this.sel_list_item.ImageIndex = -1;
            }
            grid.SelectedObject = null;
            this.sel_list_item = null;

            if (lstSections.SelectedItems.Count == 0) return;

            // Select new
            ExtraListItem item = (ExtraListItem)lstSections.SelectedItems[0];
            grid.SelectedObject = item.Extra;
            item.ImageIndex = 0;

            this.sel_list_item = item;
        }
    }
}
