using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    public class particletype_ui_editor : System.Drawing.Design.UITypeEditor
    {
        public override System.Drawing.Design.UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return System.Drawing.Design.UITypeEditorEditStyle.Modal;
        }

        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            ParticletypeList l;
            ParticletypeEdit e;

            if (value != null) {
                e = new ParticletypeEdit((particletype_item)value);
                e.ShowDialog();
                return e.Item;
            }
            
            l = new ParticletypeList();
            return l.SelectItem();
        }
    }
}
