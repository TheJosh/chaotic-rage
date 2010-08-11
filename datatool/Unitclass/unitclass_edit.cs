using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
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
            this.txtName.Text = this.item.getName();

            for (int i = 0; i < this.item.States.Count; i++) {
                ExtraListItem item = new ExtraListItem();
                item.Extra = this.item.States[i];
                item.Group = lstSettings.Groups[0];
                this.lstSettings.Items.Add(item);
            }

            for (int i = 0; i < this.item.Settings.Count; i++) {
                ExtraListItem item = new ExtraListItem();
                item.Extra = this.item.Settings[i];
                item.Group = lstSettings.Groups[1];
                this.lstSettings.Items.Add(item);
            }
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
            if (e.ChangedItem.Label == "Name") {
                this.sel_list_item.Text = e.ChangedItem.Value.ToString();
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        private void btnActions_Click(object sender, EventArgs e)
        {
            ActionsEditor f = new ActionsEditor(this.item);
            f.ShowDialog();
        }

        private void stateToolStripMenuItem_Click(object sender, EventArgs e)
        {
            unitclass_state state = new unitclass_state();

            this.item.States.Add(state);

            ExtraListItem item = new ExtraListItem();
            item.Extra = state;
            item.Group = lstSettings.Groups[0];
            this.lstSettings.Items.Add(item);
        }

        private void settingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            unitclass_settings settings = new unitclass_settings();

            this.item.Settings.Add(settings);

            ExtraListItem item = new ExtraListItem();
            item.Extra = settings;
            item.Group = lstSettings.Groups[1];
            this.lstSettings.Items.Add(item);
        }

        private void toolStripSplitButton1_ButtonClick(object sender, EventArgs e)
        {
            if (lstSettings.SelectedItems.Count == 0) return;
            base_item b = ((ExtraListItem)lstSettings.SelectedItems[0]).Extra;

            if (b is unitclass_state) { stateToolStripMenuItem_Click(sender, e); return; }
            if (b is unitclass_settings) { settingsToolStripMenuItem_Click(sender, e); return; }
        }
    }

}
