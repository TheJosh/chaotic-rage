using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class ActionsEditor : Form
    {
        private actions_item item;
        private ActionListItem currlistitem = null;

        public ActionsEditor(actions_item item)
        {
            InitializeComponent();
            this.item = item;
        }

        private void ActionsEditor_Load(object sender, EventArgs e)
        {
            for (int i = 0; i < this.item.getActions().Count; i++) {
                ActionListItem listitem = new ActionListItem();
                listitem.Action = this.item.getActions()[i];

                listitem.Text = listitem.Action.Event.ToString();
                listitem.SubItems.Add(listitem.Action.Type.ToString());

                lstActions.Items.Add(listitem);
            }
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            action a = new action();
            this.item.addAction(a);

            ActionListItem listitem = new ActionListItem();
            listitem.Action = a;
            listitem.Text = listitem.Action.Event.ToString();
            listitem.SubItems.Add(listitem.Action.Type.ToString());

            lstActions.Items.Add(listitem);
            listitem.Selected = true;
        }

        private void grid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (e.ChangedItem.Label == "Event") {
                currlistitem.SubItems[0].Text = e.ChangedItem.Value.ToString();

            } else if (e.ChangedItem.Label == "Type") {
                currlistitem.SubItems[1].Text = e.ChangedItem.Value.ToString();

                action_type type = (action_type)e.ChangedItem.Value;
                action curr = (action) grid.SelectedObject;
                action nu = null;

                switch (type) {
                    case action_type.ADD_PGENERATOR:
                        nu = new action_pgenerator(curr.Event);
                        break;

                    default:
                        nu = new action();
                        nu.Event = curr.Event;
                        break;
                }

                if (nu == null) return;

                grid.SelectedObject = nu;
                this.item.replaceAction(curr, nu);
                currlistitem.Action = nu;
            }
        }

        private void lstActions_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (currlistitem != null) {
                currlistitem.ImageIndex = -1;
            }

            if (lstActions.SelectedItems.Count == 0) return;

            currlistitem = (ActionListItem)lstActions.SelectedItems[0];
            grid.SelectedObject = currlistitem.Action;
            currlistitem.ImageIndex = 0;
        }
    }

    public class ActionListItem : ListViewItem
    {
        public action Action;
    }
}
