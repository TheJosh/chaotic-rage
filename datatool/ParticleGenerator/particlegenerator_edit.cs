using System;
using System.Collections.Generic;
using System.ComponentModel;

using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class ParticleGeneratorEdit : Form
    {
        private particlegenerator_item item;
        private ExtraListItem sel_list_item;

        public particlegenerator_item Item
        {
            get { return this.item; }
        }

        public ParticleGeneratorEdit(particlegenerator_item item)
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
            this.grid.SelectedObject = this.item;

            for (int i = 0; i < this.item.Spewers.Count; i++) {
                ExtraListItem item = new ExtraListItem();
                item.Extra = this.item.Spewers[i];
                item.Group = this.lstSections.Groups[0];
                this.lstSections.Items.Add(item);
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

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        /**
         * Edit base item
         **/
        private void editbase_Click(object sender, EventArgs e)
        {
            lstSections.SelectedItems.Clear();
            grid.SelectedObject = item;
        }

        /**
         * Something selected
         **/
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

        /**
         * Name changed
         ***/
        private void gridSpew_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (e.ChangedItem.Label == "Name" && this.sel_list_item != null)
            {
                this.sel_list_item.Text = e.ChangedItem.Value.ToString();
            }
        }


        /**
         * Add button (spewrer)
         **/
        private void spewerToolStripMenuItem_Click(object sender, EventArgs e)
        {
            particlegenerator_spew spew = new particlegenerator_spew();

            this.item.Spewers.Add(spew);

            ExtraListItem item = new ExtraListItem();
            item.Extra = spew;
            item.Group = this.lstSections.Groups[0];
            this.lstSections.Items.Add(item);
        }

    }

}
