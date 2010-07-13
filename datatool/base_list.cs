using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class BaseList : Form
    {
        private bool selectable = false;
        private base_item selected_item;

        private int icon_list_index;

        public BaseList(int icon_list_index)
        {
            this.icon_list_index = icon_list_index;
            InitializeComponent();
        }

        private void AreatypesList_Load(object sender, EventArgs e)
        {
            if (this.selectable) {
                this.selectToolStripMenuItem.Visible = true;
                this.cancelToolStripMenuItem.Visible = true;
                this.listview.DoubleClick -= new EventHandler(editToolStripMenuItem_Click);
                this.listview.DoubleClick += new EventHandler(selectToolStripMenuItem_Click);
            }
        }


        /**
         * Shows a dialog box using this form to allow the selection of an item
         **/
        public base_item SelectItem()
        {
            this.selectable = true;
            if (this.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
                return this.selected_item;
            }

            return null;
        }


        /**
         * Populates the listview with items
         **/
        protected void populateList(List<base_item> items)
        {
            this.listview.Items.Clear();

            foreach (base_item i in items) {
                LinkedListViewItem lvi;

                lvi = new LinkedListViewItem();
                lvi.Text = i.getName();
                lvi.Item = i;
                lvi.ImageIndex = this.icon_list_index;
                this.listview.Items.Add(lvi);
            }
        }


        /**
         * Add button clicked
         **/
        private void addToolStripMenuItem_Click(object sender, EventArgs e)
        {
            base_item i;
            LinkedListViewItem lvi;

            i = this.doAdd();
            if (i == null) return;

            lvi = new LinkedListViewItem();
            lvi.Text = i.getName();
            lvi.Item = i;
            lvi.ImageIndex = this.icon_list_index;
            this.listview.Items.Add(lvi);
        }

        virtual protected base_item doAdd() { return null; }


        /**
         * Edit button clicked
         **/
        private void editToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (this.listview.SelectedItems.Count == 0) return;

            LinkedListViewItem lvi = (LinkedListViewItem) this.listview.SelectedItems[0];

            lvi.Item = this.doEdit(lvi.Item);

            lvi.Text = lvi.Item.getName();
        }

        virtual protected base_item doEdit(base_item item) { return null; }


        /**
         * Delete button clicked
         **/
        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (this.listview.SelectedItems.Count == 0) return;

            this.listview.SelectedItems[0].Remove();
        }


        /**
         * Select button clicked
         **/
        private void selectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (this.listview.SelectedItems.Count == 0) return;

            LinkedListViewItem lvi = (LinkedListViewItem)this.listview.SelectedItems[0];

            this.selected_item = lvi.Item;
            this.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.Close();
        }

        private void cancelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.Close();
        }
    }


    public class LinkedListViewItem : ListViewItem
    {
        private base_item item;

        public base_item Item
        {
            get
            {
                return this.item;
            }
            set
            {
                this.item = value;
            }
        }
    }
}
