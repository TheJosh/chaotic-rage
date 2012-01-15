using System;
using System.Collections.Generic;
using System.ComponentModel;

using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace datatool
{
    public partial class ParticleGeneratorList : BaseList
    {
        public ParticleGeneratorList() : base(1, Program.dp.ParticleGenerators)
        {
            InitializeComponent();
        }

        /**
        * Shows add form
        **/
        override protected base_item doAdd()
        {
            ParticleGeneratorEdit form;

            particlegenerator_item item = new particlegenerator_item("No name");

            form = new ParticleGeneratorEdit(item);
            if (form.ShowDialog() == System.Windows.Forms.DialogResult.Cancel) return null;

            return form.Item;
        }

        /**
         * Shows edit form
         **/
        override protected base_item doEdit(base_item item)
        {
            ParticleGeneratorEdit form;
            particlegenerator_item editing = ObjectCopier.Clone<particlegenerator_item>((particlegenerator_item)item);

            form = new ParticleGeneratorEdit((particlegenerator_item)editing);
            if (form.ShowDialog() == DialogResult.Cancel) return null;

            return editing;
        }

    }
}
