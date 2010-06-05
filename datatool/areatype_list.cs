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
        public AreatypeList()
        {
            InitializeComponent();
        }

        private void AreatypeList_Load(object sender, EventArgs e)
        {
            List<base_item> list;

            list = new List<base_item>();
            list.Add(new areatype_item("Pizza"));
            list.Add(new areatype_item("Warren"));

            this.populateList(list);
        }


        /**
         * Shows add form
         **/
        override protected base_item doAdd()
        {
            MessageBox.Show("Add");
            return new areatype_item("Dodgemcar");
        }

        /**
         * Shows edit form
         **/
        override protected void doEdit(base_item item)
        {
            AreatypeEdit form;

            form = new AreatypeEdit((areatype_item) item);
            form.ShowDialog();
        }
    }
}
