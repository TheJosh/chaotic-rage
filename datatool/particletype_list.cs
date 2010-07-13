using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class ParticletypeList : BaseList
    {
        public ParticletypeList() : base(1)
        {
            InitializeComponent();
        }

        private void ParticletypeList_Load(object sender, EventArgs e)
        {
            List<base_item> list;

            list = new List<base_item>();
            list.Add(new particletype_item("Pizza"));
            list.Add(new particletype_item("Warren"));

            this.populateList(list);
        }


        /**
         * Shows add form
         **/
        override protected base_item doAdd()
        {
            ParticletypeEdit form;

            particletype_item item = new particletype_item("No name");

            form = new ParticletypeEdit(item);
            form.ShowDialog();

            return form.Item;
        }

        /**
         * Shows edit form
         **/
        override protected base_item doEdit(base_item item)
        {
            ParticletypeEdit form;

            form = new ParticletypeEdit((particletype_item)item);
            form.ShowDialog();

            return form.Item;
        }
    }
}
