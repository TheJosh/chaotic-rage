using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class ParticleGeneratorEdit : Form
    {
        private particlegenerator_item item;
        private particlegenerator_item internal_item;
        private SpewListItem sel_list_item;

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
            this.internal_item = this.item.clone();

            this.Text = this.item.getName();
            this.txtName.Text = this.item.getName();

            for (int i = 0; i < this.item.Spewers.Count; i++) {
                SpewListItem item = new SpewListItem();
                item.Spew = this.item.Spewers[i];
                this.lstSpew.Items.Add(item);
            }
        }

        /**
         * Save
         **/
        private void btnSave_Click(object sender, EventArgs e)
        {
            this.item = this.internal_item;
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
            SpewListItem item = (SpewListItem)lstSpew.SelectedItems[0];
            gridSpew.SelectedObject = item.Spew;
            item.ImageIndex = 0;

            this.sel_list_item = item;
        }

        /**
         * Add button
         **/
        private void btnAddSpew_Click(object sender, EventArgs e)
        {
            particlegenerator_spew spew = new particlegenerator_spew();

            this.internal_item.Spewers.Add(spew);

            SpewListItem item = new SpewListItem();
            item.Spew = spew;
            this.lstSpew.Items.Add(item);
        }

        private void gridSpew_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (e.ChangedItem.Label == "Type") {
                this.sel_list_item.Text = e.ChangedItem.Value.ToString();
            }
        }
    }



    /**
     * Adds a property to the list item
     **/
    public class SpewListItem : ListViewItem
    {
        private particlegenerator_spew spew;

        public SpewListItem()
        {
            this.ImageIndex = -1;
        }

        public particlegenerator_spew Spew
        {
            get { return this.spew; }
            set {
                this.spew = value;
                if (value.Type != null) {
                    this.Text = value.Type.Name;
                } else {
                    this.Text = "- Nothing -";
                }
            }
        }
    }

}
