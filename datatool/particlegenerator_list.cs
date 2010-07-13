using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class ParticleGeneratorList : BaseList
    {
        public ParticleGeneratorList() : base(1)
        {
            InitializeComponent();
        }

        private void ParticleGeneratorList_Load(object sender, EventArgs e)
        {
            List<base_item> list;

            list = new List<base_item>();
            list.Add(new particlegenerator_item("Pizza"));
            list.Add(new particlegenerator_item("Warren"));

            this.populateList(list);
        }

        /**
        * Shows add form
        **/
        override protected base_item doAdd()
        {
            ParticleGeneratorEdit form;

            particlegenerator_item item = new particlegenerator_item("No name");

            form = new ParticleGeneratorEdit(item);
            form.ShowDialog();

            return form.Item;
        }

        /**
         * Shows edit form
         **/
        override protected base_item doEdit(base_item item)
        {
            ParticleGeneratorEdit form;

            form = new ParticleGeneratorEdit((particlegenerator_item)item);
            form.ShowDialog();

            return form.Item;
        }

    }
}
