using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class ModifierList : BaseList
    {
        public ModifierList() : base(1, Program.dp.Modifiers)
        {
            InitializeComponent();
        }

        /**
        * Shows add form
        **/
        override protected base_item doAdd()
        {
            ModifierEdit form;

            modifier_item item = new modifier_item("No name");

            form = new ModifierEdit(item);
            if (form.ShowDialog() == System.Windows.Forms.DialogResult.Cancel) return null;

            return form.Item;
        }

        /**
         * Shows edit form
         **/
        override protected base_item doEdit(base_item item)
        {
            ModifierEdit form;
            modifier_item editing = ObjectCopier.Clone<modifier_item>((modifier_item)item);

            form = new ModifierEdit((modifier_item)editing);
            if (form.ShowDialog() == DialogResult.Cancel) return null;

            return editing;
        }

    }
}
