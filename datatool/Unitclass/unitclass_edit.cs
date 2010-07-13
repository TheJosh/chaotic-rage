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
        private unitclass_item internal_item;
        private SettingsListItem sel_list_item;

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
            this.internal_item = this.item.clone();

            this.Text = this.item.getName();
            this.txtName.Text = this.item.getName();

            for (int i = 0; i < this.item.Settings.Count; i++) {
                SettingsListItem item = new SettingsListItem();
                item.Settings = this.item.Settings[i];
                this.lstSpew.Items.Add(item);
            }
        }

        /**
         * Save
         **/
        private void btnSave_Click(object sender, EventArgs e)
        {
            this.item = this.internal_item;
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        /**
         * An item is selected
         **/
        private void lstSpew_SelectedIndexChanged(object sender, EventArgs e)
        {
            // Deselect existing
            if (this.sel_list_item != null) {
                this.sel_list_item.ImageIndex = -1;
            }
            gridSpew.SelectedObject = null;
            this.sel_list_item = null;

            if (lstSpew.SelectedItems.Count == 0) return;

            // Select new
            SettingsListItem item = (SettingsListItem) lstSpew.SelectedItems[0];
            gridSpew.SelectedObject = item.Settings;
            item.ImageIndex = 0;

            this.sel_list_item = item;
        }

        /**
         * Add button
         **/
        private void btnAddSpew_Click(object sender, EventArgs e)
        {
            unitclass_settings settings = new unitclass_settings();

            this.internal_item.Settings.Add(settings);

            SettingsListItem item = new SettingsListItem();
            item.Settings = settings;
            this.lstSpew.Items.Add(item);
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

        private void btnRemove_Click(object sender, EventArgs e)
        {
            if (this.sel_list_item == null) return;

            this.internal_item.Settings.Remove(this.sel_list_item.Settings);
            this.sel_list_item.Remove();
            this.sel_list_item = null;
        }
    }



    /**
     * Adds a property to the list item
     **/
    public class SettingsListItem : ListViewItem
    {
        private unitclass_settings settings;

        public SettingsListItem()
        {
            this.ImageIndex = -1;
        }

        public unitclass_settings Settings
        {
            get { return this.settings; }
            set {
                this.settings = value;
                if (value.Name != null) {
                    this.Text = value.Name;
                } else {
                    this.Text = "- Nothing -";
                }
            }
        }
    }

}
