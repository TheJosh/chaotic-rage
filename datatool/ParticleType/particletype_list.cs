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
        public ParticletypeList() : base(1, Program.dp.ParticleTypes)
        {
            InitializeComponent();
        }

        /**
         * Shows add form
         **/
        override protected base_item doAdd()
        {
            ParticletypeEdit form;

            particletype_item item = new particletype_item("No name");

            form = new ParticletypeEdit(item);
            if (form.ShowDialog() == System.Windows.Forms.DialogResult.Cancel) return null;

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
