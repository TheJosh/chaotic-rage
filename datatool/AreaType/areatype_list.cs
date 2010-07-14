using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class AreatypeList : BaseList
    {
        public AreatypeList() : base(0, Program.dp.AreaTypes)
        {
            InitializeComponent();
        }


        /**
         * Shows add form
         **/
        override protected base_item doAdd()
        {
            AreatypeEdit form;

            areatype_item item = new areatype_item("No name");

            form = new AreatypeEdit(item);
            if (form.ShowDialog() == DialogResult.Cancel) return null;

            return form.Item;
        }

        /**
         * Shows edit form
         **/
        override protected base_item doEdit(base_item item)
        {
            AreatypeEdit form;

            form = new AreatypeEdit((areatype_item) item);
            if (form.ShowDialog() == DialogResult.Cancel) return null;

            return form.Item;
        }
    }
}
