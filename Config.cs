using System.ComponentModel;
using SH_Mouse_Camera.Template.Configuration;
using Reloaded.Mod.Interfaces.Structs;
using System.ComponentModel.DataAnnotations;

namespace SH_Mouse_Camera.Configuration;

public class Config : Configurable<Config>
{
    
        [DisplayName("Sensitivity")]
        [Description("Camera Sensitivity")]
        [DefaultValue(10.0f)]
        public float Sensitivity { get; set; } = 10.0f;


        [DisplayName("Radius")]
        [Description("Radius which between camera and character")]
        [DefaultValue(70.0f)]
        public float Radius { get; set; } = 70.0f;
}

/// <summary>
/// Allows you to override certain aspects of the configuration creation process (e.g. create multiple configurations).
/// Override elements in <see cref="ConfiguratorMixinBase"/> for finer control.
/// </summary>
public class ConfiguratorMixin : ConfiguratorMixinBase
{
    // 
}
