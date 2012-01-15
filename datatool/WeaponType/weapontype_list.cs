using System;
using System.Collections.Generic;
using System.ComponentModel;

using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class WeapontypeList : BaseList
    {
        public WeapontypeList() : base(2, Program.dp.WeaponTypes)
        {
            InitializeComponent();
        }

        /**
         * Shows add form
         **/
        override protected base_item doAdd()
        {
            WeapontypeEdit form;

            weapontype_item item = new weapontype_item("No name");

            form = new WeapontypeEdit(item);
            if (form.ShowDialog() == DialogResult.Cancel) return null;

            return form.Item;
        }

        /**
         * Shows edit form
         **/
        override protected base_item doEdit(base_item item)
        {
            WeapontypeEdit form;
            weapontype_item editing = ObjectCopier.Clone<weapontype_item>((weapontype_item)item);

            form = new WeapontypeEdit((weapontype_item)editing);
            if (form.ShowDialog() == DialogResult.Cancel) return null;

            return editing;
        }
    }
}
