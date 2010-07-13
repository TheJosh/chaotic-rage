using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    public class range_ui_editor : System.Drawing.Design.UITypeEditor
    {
        public override System.Drawing.Design.UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return System.Drawing.Design.UITypeEditorEditStyle.Modal;
        }

        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            range_ui_form form = new range_ui_form();

            form.value = range.fromString((string)value);

            if (form.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
                return form.value.toString();
            }

            return value;
        }
    }
}
