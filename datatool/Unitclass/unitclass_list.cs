using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class UnitclassList : BaseList
    {
        public UnitclassList() : base(2, Program.dp.UnitClasses)
        {
            InitializeComponent();
        }

        /**
         * Shows add form
         **/
        override protected base_item doAdd()
        {
            UnitclassEdit form;

            unitclass_item item = new unitclass_item();

            form = new UnitclassEdit(item);
            if (form.ShowDialog() == DialogResult.Cancel) return null;

            return form.Item;
        }

        /**
         * Shows edit form
         **/
        override protected base_item doEdit(base_item item)
        {
            UnitclassEdit form;
            unitclass_item editing = ObjectCopier.Clone<unitclass_item>((unitclass_item)item);

            form = new UnitclassEdit((unitclass_item)editing);
            if (form.ShowDialog() == DialogResult.Cancel) return null;

            return editing;
        }
    }
}
