using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class GeneralEdit : Form
    {
        private List<base_item> items;
        private string title;
        private Type type;

        public GeneralEdit(List<base_item> items, string title, Type type)
        {
            InitializeComponent();
            this.items = items;
            this.title = title;
            this.type = type;
        }

        private void GeneralEdit_Load(object sender, EventArgs e)
        {
            this.Text = this.title;
            list.Clear();
            foreach (base_item i in items) {
                list.Items.Add(new ExtraListItem(i));
            }
        }

        private void list_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (list.SelectedItems.Count == 0) {
                grid.SelectedObject = null;
                this.Text = this.title;
                return;
            }

            ExtraListItem listitem = (ExtraListItem)list.SelectedItems[0];
            grid.SelectedObject = listitem.Extra;
            this.Text = this.title + " - " + listitem.Extra.getName();
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            base_item item = (base_item) type.GetConstructor(new Type[0]).Invoke(new Object[0]);

            ExtraListItem listitem = new ExtraListItem(item);
            items.Add(item);
            list.Items.Add(listitem);
            listitem.Selected = true;
            grid.Focus();
        }

        private void grid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (list.SelectedItems.Count == 0) {
                return;
            }

            ExtraListItem listitem = (ExtraListItem) list.SelectedItems[0];
            listitem.Text = listitem.Extra.getName();
            this.Text = this.title + " - " + listitem.Extra.getName();
        }

        private void btnRemove_Click(object sender, EventArgs e)
        {
            if (list.SelectedItems.Count == 0) {
                return;
            }

            ExtraListItem listitem = (ExtraListItem)list.SelectedItems[0];
            list.SelectedItems.Clear();

            items.Remove(listitem.Extra);
            list.Items.Remove(listitem);
        }
    }
}
