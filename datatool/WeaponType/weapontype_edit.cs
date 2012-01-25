using System;
using System.Collections.Generic;
using System.ComponentModel;

using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class WeapontypeEdit : Form
    {
        private weapontype_item item;
        private ExtraListItem sel_list_item;

        public weapontype_item Item
        {
            get { return this.item; }
        }

        public WeapontypeEdit(weapontype_item item)
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
            this.txtName.Text = this.item.getName();

            for (int i = 0; i < this.item.Sounds.Count; i++) {
                ExtraListItem item = new ExtraListItem();
                item.Extra = this.item.Sounds[i];
                item.Group = lstSettings.Groups[0];
                this.lstSettings.Items.Add(item);
            }

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

        /**
         * Settings item selected
         **/
        private void lstSettings_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (this.sel_list_item != null)
            {
                this.sel_list_item.ImageIndex = -1;
            }
            grid.SelectedObject = null;
            this.sel_list_item = null;

            if (lstSettings.SelectedItems.Count == 0) return;

            // Select new
            ExtraListItem item = (ExtraListItem) lstSettings.SelectedItems[0];
            grid.SelectedObject = item.Extra;
            item.ImageIndex = 0;

            this.sel_list_item = item;
        }

        private void gridSpew_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (this.sel_list_item == null) return;
            if (e.ChangedItem.Label == "Name") {
                this.sel_list_item.Text = e.ChangedItem.Value.ToString();
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        private void toolStripSplitButton1_ButtonClick(object sender, EventArgs e)
        {
            if (lstSettings.SelectedItems.Count == 0) return;
            base_item b = ((ExtraListItem)lstSettings.SelectedItems[0]).Extra;

            if (b is weapontype_sound) { toolStripMenuItem1_Click(sender, e); return; }
        }

        private void toolStripMenuItem1_Click(object sender, EventArgs e)
        {
            weapontype_sound state = new weapontype_sound();

            this.item.Sounds.Add(state);

            ExtraListItem item = new ExtraListItem();
            item.Extra = state;
            item.Group = lstSettings.Groups[0];
            this.lstSettings.Items.Add(item);
        }

        private void toolStripButton2_Click(object sender, EventArgs e)
        {
            lstSettings.SelectedItems.Clear();
            grid.SelectedObject = item;
        }
    }

}
