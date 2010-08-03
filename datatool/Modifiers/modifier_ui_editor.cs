using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    public class modifier_ui_editor : System.Drawing.Design.UITypeEditor
    {
        public override System.Drawing.Design.UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return System.Drawing.Design.UITypeEditorEditStyle.Modal;
        }

        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            ModifierList form;

            form = new ModifierList();
            return form.SelectItem();
        }
    }
}
