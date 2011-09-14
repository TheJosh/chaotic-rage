using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    public class image_ui_editor : System.Drawing.Design.UITypeEditor
    {
        public override System.Drawing.Design.UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return System.Drawing.Design.UITypeEditorEditStyle.Modal;
        }

        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            image_ui_form form = new image_ui_form();

            if (context.Instance is particletype_item) {
                form.directory = "particletypes";
            } else if (context.Instance is unitclass_state) {
                form.directory = "unitclass";
            } else {
                return "ERROR: Unknown class";
            }

            form.value = (string) value;

            if (form.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
                return form.value;
            }

            return value;
        }
    }
}
