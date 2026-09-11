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
		
        [DisplayName("Camera Position Smooth Factor (Experimental)")]
        [Description("Camera Position smoother\n 1: close")]
        [DefaultValue(1.0)]
        [SliderControlParams(minimum: 0.0, maximum: 1.0)]
        public double CameraPositionSmoothFactor { get; set; } = 1.0;
		
        [DisplayName("Camera Rotation Smooth Factor (Experimental)")]
        [Description("Camera Rotation smoother\n 1: close")]
        [DefaultValue(1.0)]
        [SliderControlParams(minimum: 0.0, maximum: 1.0)]
        public double CameraRotationSmoothFactor { get; set; } = 1.0;
		
        [DisplayName("Cursor Hiding")]
        [Description("Cursor is hiding at game window")]
        [DefaultValue(false)]
        public bool HideCursor { get; set; } = false;

        [DisplayName("Cursor Locking")]
        [Description("Cursor is locked at a resolution of 400x300")]
        [DefaultValue(false)]
        public bool CursorLock { get; set; } = false;
		
        [DisplayName("Invert X Axis")]
        [Description("Inverts X Axis")]
        [DefaultValue(false)]
        public bool InvertXAxis { get; set; } = false;
		
        [DisplayName("Invert Y Axis")]
        [Description("Inverts Y Axis")]
        [DefaultValue(false)]
        public bool InvertYAxis { get; set; } = false;
}

/// <summary>
/// Allows you to override certain aspects of the configuration creation process (e.g. create multiple configurations).
/// Override elements in <see cref="ConfiguratorMixinBase"/> for finer control.
/// </summary>
public class ConfiguratorMixin : ConfiguratorMixinBase
{
    // 
}
