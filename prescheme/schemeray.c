#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "prescheme.h"
#include <stdlib.h>
#include <math.h>
#include "c-util.h"

struct vector2d {
  double x;
  double y;
};
struct vector3d {
  double x;
  double y;
  double z;
};
struct object {
  long type;
  struct vector3d *color;
};
struct light {
  long type;
  struct vector3d *color;
  struct vector3d *position;
  double radius;
};
struct sphere {
  long type;
  struct vector3d *color;
  struct vector3d *position;
  double radius;
};
struct plane {
  long type;
  struct vector3d *color;
  struct vector3d *normal;
  double distance;
};
struct triangle {
  long type;
  struct vector3d *color;
  struct vector3d *vertex1;
  struct vector3d *vertex2;
  struct vector3d *vertex3;
};
struct mesh {
  long type;
  struct triangle **triangles;
};
static double HflS30(double, double);
static double Hfl_21(double, double);
static double HflA12(double, double);
static double vector3d_dot(struct vector3d*, struct vector3d*);
static struct vector3d *vector3d_op(struct vector3d*, struct vector3d*, double(*)(double, double));
static struct vector3d *vector3d_scalar_mul(struct vector3d*, double);
static struct vector3d *vector3d_cross(struct vector3d*, struct vector3d*);
static struct vector3d *triangle_normal(struct triangle*);
static struct vector3d *shoot_ray(struct vector3d*, struct vector3d*, long);
long main(long, char**);
static struct vector3d *ambient;
static struct vector3d *point;
static struct vector3d *eye;
static struct vector3d *default_color;
static struct object **scene;
static long scene_size;

static double HflS30(double x_3X, double y_4X)
{

 {  return (x_3X * y_4X);}
}
static double Hfl_21(double x_5X, double y_6X)
{

 {  return (x_5X - y_6X);}
}
static double HflA12(double x_7X, double y_8X)
{

 {  return (x_7X + y_8X);}
}
static double vector3d_dot(struct vector3d *v1_9X, struct vector3d *v2_10X)
{

 {  return ((((v1_9X->x) * (v2_10X->x)) + ((v1_9X->y) * (v2_10X->y))) + ((v1_9X->z) * (v2_10X->z)));}
}
static struct vector3d *vector3d_op(struct vector3d *v1_11X, struct vector3d *v2_12X, double (*op_13X)(double, double))
{
  struct vector3d *vector3d_17X;
  double z_16X;
  double x_15X;
  double y_14X;
 {  y_14X = (*op_13X)((v1_11X->y), (v2_12X->y));
  x_15X = (*op_13X)((v1_11X->x), (v2_12X->x));
  z_16X = (*op_13X)((v1_11X->z), (v2_12X->z));
  vector3d_17X = (struct vector3d*)malloc(sizeof(struct vector3d));
  if ((NULL == vector3d_17X)) {
    return vector3d_17X;}
  else {
    vector3d_17X->x = x_15X;
    vector3d_17X->y = y_14X;
    vector3d_17X->z = z_16X;
    return vector3d_17X;}}
}
static struct vector3d *vector3d_scalar_mul(struct vector3d *v1_18X, double f_19X)
{
  struct vector3d *vector3d_23X;
  double v_22X;
  double v_21X;
  double v_20X;
 {  v_20X = v1_18X->y;
  v_21X = v1_18X->x;
  v_22X = v1_18X->z;
  vector3d_23X = (struct vector3d*)malloc(sizeof(struct vector3d));
  if ((NULL == vector3d_23X)) {
    return vector3d_23X;}
  else {
    vector3d_23X->x = (v_21X * f_19X);
    vector3d_23X->y = (v_20X * f_19X);
    vector3d_23X->z = (v_22X * f_19X);
    return vector3d_23X;}}
}
static struct vector3d *vector3d_cross(struct vector3d *v1_24X, struct vector3d *v2_25X)
{
  struct vector3d *vector3d_33X;
  double v_32X;
  double v2_z_31X;
  double v1_x_30X;
  double v2_x_29X;
  double v1_y_28X;
  double v2_y_27X;
  double v1_z_26X;
 {  v1_z_26X = v1_24X->z;
  v2_y_27X = v2_25X->y;
  v1_y_28X = v1_24X->y;
  v2_x_29X = v2_25X->x;
  v1_x_30X = v1_24X->x;
  v2_z_31X = v2_25X->z;
  v_32X = v2_25X->z;
  vector3d_33X = (struct vector3d*)malloc(sizeof(struct vector3d));
  if ((NULL == vector3d_33X)) {
    return vector3d_33X;}
  else {
    vector3d_33X->x = (((v1_y_28X * v2_z_31X) * v_32X) - (v1_z_26X * v2_y_27X));
    vector3d_33X->y = ((v1_z_26X * v2_x_29X) - (v1_x_30X * v2_z_31X));
    vector3d_33X->z = ((v1_x_30X * v2_y_27X) - (v1_y_28X * v2_x_29X));
    return vector3d_33X;}}
}
static struct vector3d *triangle_normal(struct triangle *triangle_34X)
{
  double v_39X;
  double v_38X;
  struct vector3d *v1_37X;
  struct vector3d *v_36X;
  struct vector3d *v_35X;
 {  v_35X = vector3d_op((triangle_34X->vertex2), (triangle_34X->vertex1), Hfl_21);
  v_36X = vector3d_op((triangle_34X->vertex3), (triangle_34X->vertex1), Hfl_21);
  v1_37X = vector3d_cross(v_35X, v_36X);
  v_38X = vector3d_dot(v1_37X, v1_37X);
  v_39X = sqrt(v_38X);
  return vector3d_scalar_mul(v1_37X, (0.99999999 / v_39X));}
}
static struct vector3d *shoot_ray(struct vector3d *orig_40X, struct vector3d *dir_41X, long step_42X)
{
  struct vector3d *arg3K1;
  struct vector3d *arg3K0;
  struct object *arg2K1;
  long arg1K0;
  double arg0K2;
  double arg0K0;
  double merged_arg0K1;
  double merged_arg0K0;
  struct vector3d *merged_arg3K1;
  struct object *merged_arg2K0;

  int object_normal_return_tag;
  struct vector3d *object_normal0_return_value;
  int same_signPD0_return_tag;
  char same_signPD00_return_value;
  struct object *object_43X;
  struct vector3d *point_44X;
  double x_45X;
  double y_46X;
  struct vector3d *v_153X;
  double v_152X;
  double v_151X;
  struct vector3d *v1_150X;
  struct sphere *sphere_149X;
  struct vector3d *vector3d_148X;
  struct vector3d *v_147X;
  struct triangle *v_146X;
  struct plane *plane_145X;
  char temp_144X;
  struct vector3d *v_143X;
  struct vector3d *v2_142X;
  struct vector3d *v2_141X;
  struct vector3d *v_140X;
  struct vector3d *v1_139X;
  struct vector3d *v2_138X;
  struct vector3d *vector3d_137X;
  struct vector3d *vector3d_136X;
  char v_135X;
  double shadow_134X;
  double n_133X;
  double spec_132X;
  double rDv_131X;
  double n_130X;
  struct vector3d *r_129X;
  struct vector3d *v2_128X;
  double v_127X;
  double nDl_126X;
  struct vector3d *v1_125X;
  double n_124X;
  struct vector3d *n_123X;
  struct vector3d *l_122X;
  double v_121X;
  double v_120X;
  struct vector3d *point_to_light_119X;
  struct light *light_118X;
  struct object *obj_117X;
  struct vector3d *acc_116X;
  long i_115X;
  struct vector3d *v_114X;
  double d_113X;
  double r_112X;
  double v_111X;
  double d_110X;
  double v_109X;
  double v_108X;
  double b_107X;
  struct vector3d *eo_106X;
  struct sphere *obj_105X;
  struct vector3d *vector3d_104X;
  struct vector3d *v2_103X;
  double v_102X;
  struct vector3d *v_101X;
  struct vector3d *v_100X;
  struct vector3d *v2_99X;
  struct vector3d *view_98X;
  double v_97X;
  double v_96X;
  struct vector3d *v1_95X;
  struct vector3d *r_94X;
  double v_93X;
  double v_92X;
  struct vector3d *v1_91X;
  struct vector3d *v2_90X;
  double v_89X;
  struct vector3d *n_88X;
  struct vector3d *point_87X;
  struct vector3d *v2_86X;
  struct vector3d *vector3d_85X;
  char v_84X;
  char v_83X;
  double v_82X;
  struct vector3d *v_81X;
  struct vector3d *v_80X;
  struct vector3d *v_79X;
  char v_78X;
  double v_77X;
  struct vector3d *v_76X;
  struct vector3d *v_75X;
  struct vector3d *v_74X;
  double val_73X;
  struct vector3d *v_72X;
  struct vector3d *v_71X;
  struct vector3d *v_70X;
  struct vector3d *p_69X;
  struct vector3d *v2_68X;
  double d_67X;
  double o_aDn_66X;
  struct vector3d *v_65X;
  struct vector3d *c_64X;
  struct vector3d *b_63X;
  struct vector3d *a_62X;
  double vDn_61X;
  struct vector3d *n_60X;
  struct triangle *obj_59X;
  double dist_58X;
  double v_57X;
  double d_56X;
  struct plane *obj_55X;
  char temp_54X;
  struct object *obj_53X;
  double closest_depth_52X;
  struct object *closest_object_51X;
  long i_50X;
  struct vector3d *vector3d_49X;
  long num_prims_48X;
  struct object **prims_47X;
 {  if ((step_42X < 3)) {
    prims_47X = scene;
    num_prims_48X = scene_size;
    arg1K0 = 0;
    arg2K1 = NULL;
    arg0K2 = 10000.0;
    goto L2146;}
  else {
    vector3d_49X = (struct vector3d*)malloc(sizeof(struct vector3d));
    if ((NULL == vector3d_49X)) {
      return vector3d_49X;}
    else {
      vector3d_49X->x = 0.0;
      vector3d_49X->y = 0.0;
      vector3d_49X->z = 0.0;
      return vector3d_49X;}}}
 L2146: {
  i_50X = arg1K0;
  closest_object_51X = arg2K1;
  closest_depth_52X = arg0K2;
  if ((i_50X < num_prims_48X)) {
    obj_53X = *(prims_47X + i_50X);
    temp_54X = 0 == (obj_53X->type);
    if (temp_54X) {
      goto L2089;}
    else {
      if ((1 == (obj_53X->type))) {
        goto L2089;}
      else {
        if ((2 == (obj_53X->type))) {
          obj_55X = (struct plane*)(obj_53X);
          d_56X = vector3d_dot((obj_55X->normal), dir_41X);
          if ((0.0 == d_56X)) {
            arg0K0 = 10000.0;
            goto L2151;}
          else {
            v_57X = vector3d_dot((obj_55X->normal), orig_40X);
            dist_58X = (0.0 - (v_57X + (obj_55X->distance))) / d_56X;
            if ((0.0 < dist_58X)) {
              arg0K0 = dist_58X;
              goto L2151;}
            else {
              arg0K0 = 10000.0;
              goto L2151;}}}
        else {
          if ((3 == (obj_53X->type))) {
            obj_59X = (struct triangle*)(obj_53X);
            n_60X = triangle_normal(obj_59X);
            vDn_61X = vector3d_dot(dir_41X, n_60X);
            if ((vDn_61X < 0.0)) {
              a_62X = obj_59X->vertex1;
              b_63X = obj_59X->vertex2;
              c_64X = obj_59X->vertex3;
              v_65X = vector3d_op(orig_40X, a_62X, Hfl_21);
              o_aDn_66X = vector3d_dot(v_65X, n_60X);
              d_67X = 0.0 - (o_aDn_66X / vDn_61X);
              v2_68X = vector3d_scalar_mul(dir_41X, d_67X);
              p_69X = vector3d_op(orig_40X, v2_68X, HflA12);
              v_70X = vector3d_op(b_63X, a_62X, Hfl_21);
              v_71X = vector3d_op(p_69X, a_62X, Hfl_21);
              v_72X = vector3d_cross(v_70X, v_71X);
              val_73X = vector3d_dot(v_72X, n_60X);
              if ((d_67X < 0.0)) {
                arg0K0 = 10000.0;
                goto L2151;}
              else {
                v_74X = vector3d_op(c_64X, b_63X, Hfl_21);
                v_75X = vector3d_op(p_69X, b_63X, Hfl_21);
                v_76X = vector3d_cross(v_74X, v_75X);
                v_77X = vector3d_dot(v_76X, n_60X);
                merged_arg0K0 = val_73X;
                merged_arg0K1 = v_77X;
                same_signPD0_return_tag = 0;
                goto same_signPD0;
               same_signPD0_return_0:
                v_78X = same_signPD00_return_value;
                if (v_78X) {
                  v_79X = vector3d_op(a_62X, c_64X, Hfl_21);
                  v_80X = vector3d_op(p_69X, c_64X, Hfl_21);
                  v_81X = vector3d_cross(v_79X, v_80X);
                  v_82X = vector3d_dot(v_81X, n_60X);
                  merged_arg0K0 = val_73X;
                  merged_arg0K1 = v_82X;
                  same_signPD0_return_tag = 1;
                  goto same_signPD0;
                 same_signPD0_return_1:
                  v_83X = same_signPD00_return_value;
                  if (v_83X) {
                    arg0K0 = d_67X;
                    goto L2151;}
                  else {
                    arg0K0 = 10000.0;
                    goto L2151;}}
                else {
                  arg0K0 = 10000.0;
                  goto L2151;}}}
            else {
              arg0K0 = 10000.0;
              goto L2151;}}
          else {
            arg0K0 = 10000.0;
            goto L2151;}}}}}
  else {
    v_84X = ISNULL(closest_object_51X);
    if (v_84X) {
      vector3d_85X = (struct vector3d*)malloc(sizeof(struct vector3d));
      if ((NULL == vector3d_85X)) {
        return vector3d_85X;}
      else {
        vector3d_85X->x = 0.0;
        vector3d_85X->y = 0.0;
        vector3d_85X->z = 0.0;
        return vector3d_85X;}}
    else {
      v2_86X = vector3d_scalar_mul(dir_41X, closest_depth_52X);
      point_87X = vector3d_op(orig_40X, v2_86X, HflA12);
      merged_arg2K0 = closest_object_51X;
      merged_arg3K1 = point_87X;
      object_normal_return_tag = 0;
      goto object_normal;
     object_normal_return_0:
      n_88X = object_normal0_return_value;
      v_89X = vector3d_dot(dir_41X, n_88X);
      v2_90X = vector3d_scalar_mul(n_88X, (2.0 * v_89X));
      v1_91X = vector3d_op(dir_41X, v2_90X, Hfl_21);
      v_92X = vector3d_dot(v1_91X, v1_91X);
      v_93X = sqrt(v_92X);
      r_94X = vector3d_scalar_mul(v1_91X, (0.99999999 / v_93X));
      v1_95X = vector3d_op(point_87X, (eye), Hfl_21);
      v_96X = vector3d_dot(v1_95X, v1_95X);
      v_97X = sqrt(v_96X);
      view_98X = vector3d_scalar_mul(v1_95X, (0.99999999 / v_97X));
      v2_99X = vector3d_scalar_mul(r_94X, 1e-4);
      v_100X = vector3d_op(point_87X, v2_99X, HflA12);
      v_101X = shoot_ray(v_100X, r_94X, (1 + step_42X));
      v_102X = (float)((1 + step_42X));
      v2_103X = vector3d_scalar_mul(v_101X, (0.99999999 / (v_102X * 2.0)));
      if ((0 == (closest_object_51X->type))) {
        return vector3d_op((closest_object_51X->color), v2_103X, HflA12);}
      else {
        vector3d_104X = (struct vector3d*)malloc(sizeof(struct vector3d));
        if ((NULL == vector3d_104X)) {
          arg3K0 = vector3d_104X;
          goto L1906;}
        else {
          vector3d_104X->x = 0.0;
          vector3d_104X->y = 0.0;
          vector3d_104X->z = 0.0;
          arg3K0 = vector3d_104X;
          goto L1906;}}}}}
 L2089: {
  obj_105X = (struct sphere*)(obj_53X);
  eo_106X = vector3d_op(orig_40X, (obj_105X->position), Hfl_21);
  b_107X = vector3d_dot(eo_106X, dir_41X);
  v_108X = pow((obj_105X->radius), 2.0);
  v_109X = vector3d_dot(eo_106X, eo_106X);
  d_110X = (b_107X * b_107X) - (v_109X - v_108X);
  if ((0.0 < d_110X)) {
    v_111X = sqrt(d_110X);
    r_112X = (0.0 - b_107X) - v_111X;
    if ((0.0 < r_112X)) {
      arg0K0 = r_112X;
      goto L2151;}
    else {
      arg0K0 = 10000.0;
      goto L2151;}}
  else {
    arg0K0 = 10000.0;
    goto L2151;}}
 L2151: {
  d_113X = arg0K0;
  if ((d_113X < closest_depth_52X)) {
    arg1K0 = (1 + i_50X);
    arg2K1 = obj_53X;
    arg0K2 = d_113X;
    goto L2146;}
  else {
    arg1K0 = (1 + i_50X);
    arg2K1 = closest_object_51X;
    arg0K2 = closest_depth_52X;
    goto L2146;}}
 L1906: {
  v_114X = arg3K0;
  arg1K0 = 0;
  arg3K1 = v_114X;
  goto L1815;}
 L1815: {
  i_115X = arg1K0;
  acc_116X = arg3K1;
  if ((i_115X < (scene_size))) {
    obj_117X = *((scene) + i_115X);
    if ((0 == (obj_117X->type))) {
      light_118X = (struct light*)(obj_117X);
      point_to_light_119X = vector3d_op((light_118X->position), point_87X, Hfl_21);
      v_120X = vector3d_dot(point_to_light_119X, point_to_light_119X);
      v_121X = sqrt(v_120X);
      l_122X = vector3d_scalar_mul(point_to_light_119X, (0.99999999 / v_121X));
      merged_arg2K0 = closest_object_51X;
      merged_arg3K1 = point_87X;
      object_normal_return_tag = 1;
      goto object_normal;
     object_normal_return_1:
      n_123X = object_normal0_return_value;
      n_124X = vector3d_dot(n_123X, l_122X);
      if ((n_124X < 0.0)) {
        arg0K0 = 0.0;
        goto L1843;}
      else {
        if ((1.0 < n_124X)) {
          arg0K0 = 1.0;
          goto L1843;}
        else {
          arg0K0 = n_124X;
          goto L1843;}}}
    else {
      arg1K0 = (1 + i_115X);
      arg3K1 = acc_116X;
      goto L1815;}}
  else {
    if ((2 == (closest_object_51X->type))) {
      return vector3d_op(acc_116X, v2_103X, HflA12);}
    else {
      v1_125X = vector3d_op((ambient), acc_116X, HflA12);
      return vector3d_op(v1_125X, v2_103X, HflA12);}}}
 L1843: {
  nDl_126X = arg0K0;
  v_127X = vector3d_dot(n_123X, l_122X);
  v2_128X = vector3d_scalar_mul(n_123X, (2.0 * v_127X));
  r_129X = vector3d_op(l_122X, v2_128X, Hfl_21);
  n_130X = vector3d_dot(r_129X, view_98X);
  if ((n_130X < 0.0)) {
    arg0K0 = 0.0;
    goto L1857;}
  else {
    if ((1.0 < n_130X)) {
      arg0K0 = 1.0;
      goto L1857;}
    else {
      arg0K0 = n_130X;
      goto L1857;}}}
 L1857: {
  rDv_131X = arg0K0;
  spec_132X = pow(rDv_131X, 30.0);
  n_133X = 4.0 * nDl_126X;
  if ((n_133X < 0.0)) {
    arg0K0 = 0.0;
    goto L1867;}
  else {
    if ((1.0 < n_133X)) {
      arg0K0 = 1.0;
      goto L1867;}
    else {
      arg0K0 = n_133X;
      goto L1867;}}}
 L1867: {
  shadow_134X = arg0K0;
  if ((0.0 < nDl_126X)) {
    v_135X = 2 == (closest_object_51X->type);
    if (v_135X) {
      vector3d_136X = (struct vector3d*)malloc(sizeof(struct vector3d));
      if ((NULL == vector3d_136X)) {
        arg3K0 = vector3d_136X;
        goto L1890;}
      else {
        vector3d_136X->x = 0.0;
        vector3d_136X->y = 0.0;
        vector3d_136X->z = 0.0;
        arg3K0 = vector3d_136X;
        goto L1890;}}
    else {
      vector3d_137X = (struct vector3d*)malloc(sizeof(struct vector3d));
      if ((NULL == vector3d_137X)) {
        arg3K0 = vector3d_137X;
        goto L1890;}
      else {
        vector3d_137X->x = spec_132X;
        vector3d_137X->y = spec_132X;
        vector3d_137X->z = spec_132X;
        arg3K0 = vector3d_137X;
        goto L1890;}}}
  else {
    arg1K0 = (1 + i_115X);
    arg3K1 = acc_116X;
    goto L1815;}}
 L1890: {
  v2_138X = arg3K0;
  v1_139X = vector3d_scalar_mul((closest_object_51X->color), nDl_126X);
  v_140X = vector3d_op(v1_139X, v2_138X, HflA12);
  v2_141X = vector3d_scalar_mul(v_140X, shadow_134X);
  v2_142X = vector3d_op((obj_117X->color), v2_141X, HflS30);
  v_143X = vector3d_op(acc_116X, v2_142X, HflA12);
  arg1K0 = (1 + i_115X);
  arg3K1 = v_143X;
  goto L1815;}
 same_signPD0: {
  x_45X = merged_arg0K0;
  y_46X = merged_arg0K1;{
  same_signPD00_return_value = ((x_45X < 0.0) == (y_46X < 0.0));
  goto same_signPD0_return;}
 same_signPD0_return:
  switch (same_signPD0_return_tag) {
  case 0: goto same_signPD0_return_0;
  default: goto same_signPD0_return_1;
  }}

 object_normal: {
  object_43X = merged_arg2K0;
  point_44X = merged_arg3K1;{
  temp_144X = 0 == (object_43X->type);
  if (temp_144X) {
    goto L1512;}
  else {
    if ((1 == (object_43X->type))) {
      goto L1512;}
    else {
      if ((2 == (object_43X->type))) {
        plane_145X = (struct plane*)(object_43X);
        object_normal0_return_value = (plane_145X->normal);
        goto object_normal_return;}
      else {
        if ((3 == (object_43X->type))) {
          v_146X = (struct triangle*)(object_43X);
          v_147X = triangle_normal(v_146X);
          object_normal0_return_value = v_147X;
          goto object_normal_return;}
        else {
          vector3d_148X = (struct vector3d*)malloc(sizeof(struct vector3d));
          if ((NULL == vector3d_148X)) {
            object_normal0_return_value = vector3d_148X;
            goto object_normal_return;}
          else {
            vector3d_148X->x = 0.0;
            vector3d_148X->y = 1.0;
            vector3d_148X->z = 0.0;
            object_normal0_return_value = vector3d_148X;
            goto object_normal_return;}}}}}}
 L1512: {
  sphere_149X = (struct sphere*)(object_43X);
  v1_150X = vector3d_op(point_44X, (sphere_149X->position), Hfl_21);
  v_151X = vector3d_dot(v1_150X, v1_150X);
  v_152X = sqrt(v_151X);
  v_153X = vector3d_scalar_mul(v1_150X, (0.99999999 / v_152X));
  object_normal0_return_value = v_153X;
  goto object_normal_return;}
 object_normal_return:
  switch (object_normal_return_tag) {
  case 0: goto object_normal_return_0;
  default: goto object_normal_return_1;
  }}

}
long main(long argc_154X, char **argv_155X)
{
  double arg0K0;
  struct object *arg2K0;
  long arg1K0;
  struct vector3d *arg3K0;
  double merged_arg0K0;
  struct vector3d *merged_arg3K0;

  int vector3d_inverse_return_tag;
  struct vector3d *vector3d_inverse0_return_value;
  int wD1_return_tag;
  struct vector3d *v1_156X;
  double c_157X;
  struct vector3d *vector3d_237X;
  double v_236X;
  double v_235X;
  double v_234X;
  long v_233X;
  double f_232X;
  struct object *v_231X;
  struct vector3d *color_230X;
  struct vector3d *view_229X;
  double v_228X;
  double v_227X;
  struct vector3d *v1_226X;
  struct vector3d *v1_225X;
  struct vector3d *point_224X;
  struct vector3d *v2_223X;
  double v_222X;
  struct vector3d *v1_221X;
  double v_220X;
  long y_219X;
  long x_218X;
  long n_217X;
  struct object *v_216X;
  struct object *v_215X;
  struct sphere *sphere_214X;
  struct vector3d *color_213X;
  struct vector3d *corner_212X;
  struct vector3d *v2_211X;
  struct vector3d *v_210X;
  struct vector3d *v1_209X;
  struct vector3d *v_208X;
  struct vector3d *dy_207X;
  double v_206X;
  struct vector3d *dx_205X;
  double v_204X;
  struct vector3d *eye_up_203X;
  struct vector3d *eye_right_202X;
  struct vector3d *v_201X;
  struct vector3d *vector3d_200X;
  struct vector3d *position_199X;
  struct vector3d *vector3d_198X;
  double v_197X;
  double v_196X;
  struct vector3d *v1_195X;
  struct vector3d *eye_unit_194X;
  double v_193X;
  double v_192X;
  struct vector3d *v1_191X;
  long v_190X;
  FILE * port_189X;
  struct vector3d *vector3d_188X;
  double v_187X;
  double v_186X;
  long i_185X;
  struct object *v_184X;
  struct object *v_183X;
  struct object *v_182X;
  struct plane *plane_181X;
  struct vector3d *color_180X;
  struct vector3d *vector3d_179X;
  struct vector3d *normal_178X;
  struct vector3d *vector3d_177X;
  struct object *v_176X;
  struct object *v_175X;
  struct object *v_174X;
  struct light *light_173X;
  struct vector3d *color_172X;
  struct vector3d *vector3d_171X;
  struct vector3d *position_170X;
  struct vector3d *vector3d_169X;
  struct object **objects_168X;
  struct object *v_167X;
  struct object *object_166X;
  struct vector3d *v_165X;
  struct vector3d *vector3d_164X;
  struct vector3d *v_163X;
  struct vector3d *vector3d_162X;
  struct vector3d *v_161X;
  struct vector3d *vector3d_160X;
  struct vector3d *v_159X;
  struct vector3d *vector3d_158X;
 {  vector3d_158X = (struct vector3d*)malloc(sizeof(struct vector3d));
  if ((NULL == vector3d_158X)) {
    arg3K0 = vector3d_158X;
    goto L1278;}
  else {
    vector3d_158X->x = 0.0;
    vector3d_158X->y = 0.0;
    vector3d_158X->z = 1.0;
    arg3K0 = vector3d_158X;
    goto L1278;}}
 L1278: {
  v_159X = arg3K0;
  eye = v_159X;
  vector3d_160X = (struct vector3d*)malloc(sizeof(struct vector3d));
  if ((NULL == vector3d_160X)) {
    arg3K0 = vector3d_160X;
    goto L1283;}
  else {
    vector3d_160X->x = 0.0;
    vector3d_160X->y = 0.0;
    vector3d_160X->z = -5.0;
    arg3K0 = vector3d_160X;
    goto L1283;}}
 L1283: {
  v_161X = arg3K0;
  point = v_161X;
  vector3d_162X = (struct vector3d*)malloc(sizeof(struct vector3d));
  if ((NULL == vector3d_162X)) {
    arg3K0 = vector3d_162X;
    goto L1288;}
  else {
    vector3d_162X->x = 0.0;
    vector3d_162X->y = 0.0;
    vector3d_162X->z = 0.0;
    arg3K0 = vector3d_162X;
    goto L1288;}}
 L1288: {
  v_163X = arg3K0;
  default_color = v_163X;
  vector3d_164X = (struct vector3d*)malloc(sizeof(struct vector3d));
  if ((NULL == vector3d_164X)) {
    arg3K0 = vector3d_164X;
    goto L1293;}
  else {
    vector3d_164X->x = 0.1;
    vector3d_164X->y = 0.1;
    vector3d_164X->z = 0.1;
    arg3K0 = vector3d_164X;
    goto L1293;}}
 L1293: {
  v_165X = arg3K0;
  ambient = v_165X;
  object_166X = (struct object*)malloc(sizeof(struct object));
  if ((NULL == object_166X)) {
    arg2K0 = object_166X;
    goto L1300;}
  else {
    object_166X->type = -1;
    arg2K0 = object_166X;
    goto L1300;}}
 L1300: {
  v_167X = arg2K0;
  objects_168X = (struct object**)malloc(sizeof(struct object*) * 27);
  vector3d_169X = (struct vector3d*)malloc(sizeof(struct vector3d));
  if ((NULL == vector3d_169X)) {
    arg3K0 = vector3d_169X;
    goto L1306;}
  else {
    vector3d_169X->x = -40.0;
    vector3d_169X->y = -15.0;
    vector3d_169X->z = 60.0;
    arg3K0 = vector3d_169X;
    goto L1306;}}
 L1306: {
  position_170X = arg3K0;
  vector3d_171X = (struct vector3d*)malloc(sizeof(struct vector3d));
  if ((NULL == vector3d_171X)) {
    arg3K0 = vector3d_171X;
    goto L1304;}
  else {
    vector3d_171X->x = 0.7;
    vector3d_171X->y = 0.9;
    vector3d_171X->z = 0.9;
    arg3K0 = vector3d_171X;
    goto L1304;}}
 L1304: {
  color_172X = arg3K0;
  light_173X = (struct light*)malloc(sizeof(struct light));
  if ((NULL == light_173X)) {
    v_174X = (struct object*)(light_173X);
    arg2K0 = v_174X;
    goto L1308;}
  else {
    light_173X->type = 0;
    light_173X->color = color_172X;
    light_173X->position = position_170X;
    light_173X->radius = 1.0;
    v_175X = (struct object*)(light_173X);
    arg2K0 = v_175X;
    goto L1308;}}
 L1308: {
  v_176X = arg2K0;
  *(objects_168X + 0) = v_176X;
  vector3d_177X = (struct vector3d*)malloc(sizeof(struct vector3d));
  if ((NULL == vector3d_177X)) {
    arg3K0 = vector3d_177X;
    goto L1314;}
  else {
    vector3d_177X->x = 0.0;
    vector3d_177X->y = -1.0;
    vector3d_177X->z = 0.0;
    arg3K0 = vector3d_177X;
    goto L1314;}}
 L1314: {
  normal_178X = arg3K0;
  vector3d_179X = (struct vector3d*)malloc(sizeof(struct vector3d));
  if ((NULL == vector3d_179X)) {
    arg3K0 = vector3d_179X;
    goto L1312;}
  else {
    vector3d_179X->x = 0.7;
    vector3d_179X->y = 0.7;
    vector3d_179X->z = 1.0;
    arg3K0 = vector3d_179X;
    goto L1312;}}
 L1312: {
  color_180X = arg3K0;
  plane_181X = (struct plane*)malloc(sizeof(struct plane));
  if ((NULL == plane_181X)) {
    v_182X = (struct object*)(plane_181X);
    arg2K0 = v_182X;
    goto L1316;}
  else {
    plane_181X->type = 2;
    plane_181X->color = color_180X;
    plane_181X->normal = normal_178X;
    plane_181X->distance = 5.0;
    v_183X = (struct object*)(plane_181X);
    arg2K0 = v_183X;
    goto L1316;}}
 L1316: {
  v_184X = arg2K0;
  *(objects_168X + 1) = v_184X;
  arg1K0 = 0;
  goto L1053;}
 L1053: {
  i_185X = arg1K0;
  if ((i_185X < 25)) {
    v_186X = (float)((i_185X / 5));
    v_187X = (float)((i_185X % 5));
    vector3d_188X = (struct vector3d*)malloc(sizeof(struct vector3d));
    if ((NULL == vector3d_188X)) {
      arg3K0 = vector3d_188X;
      goto L1062;}
    else {
      vector3d_188X->x = (15.0 * v_187X);
      vector3d_188X->y = 0.0;
      vector3d_188X->z = (15.0 + (15.0 * v_186X));
      arg3K0 = vector3d_188X;
      goto L1062;}}
  else {
    scene = objects_168X;
    scene_size = 27;
    port_189X = ps_open_output_file("image.ppm", &v_190X);
    ps_write_string("P6\n", port_189X);
    ps_write_integer(800, port_189X);
    ps_write_string(" ", port_189X);
    ps_write_integer(600, port_189X);
    ps_write_string("\n", port_189X);
    ps_write_string("255\n", port_189X);
    v1_191X = eye;
    v_192X = vector3d_dot(v1_191X, v1_191X);
    v_193X = sqrt(v_192X);
    eye_unit_194X = vector3d_scalar_mul(v1_191X, (0.99999999 / v_193X));
    v1_195X = point;
    v_196X = vector3d_dot(v1_195X, v1_195X);
    v_197X = sqrt(v_196X);vector3d_scalar_mul(v1_195X, (0.99999999 / v_197X));
    vector3d_198X = (struct vector3d*)malloc(sizeof(struct vector3d));
    if ((NULL == vector3d_198X)) {
      arg3K0 = vector3d_198X;
      goto L2374;}
    else {
      vector3d_198X->x = 0.0;
      vector3d_198X->y = 1.0;
      vector3d_198X->z = 0.0;
      arg3K0 = vector3d_198X;
      goto L2374;}}}
 L1062: {
  position_199X = arg3K0;
  vector3d_200X = (struct vector3d*)malloc(sizeof(struct vector3d));
  if ((NULL == vector3d_200X)) {
    arg3K0 = vector3d_200X;
    goto L1056;}
  else {
    vector3d_200X->x = 0.2;
    vector3d_200X->y = 0.5;
    vector3d_200X->z = 0.5;
    arg3K0 = vector3d_200X;
    goto L1056;}}
 L2374: {
  v_201X = arg3K0;
  eye_right_202X = vector3d_cross(eye_unit_194X, v_201X);
  eye_up_203X = vector3d_cross(eye_unit_194X, eye_right_202X);
  v_204X = (float)(800);
  dx_205X = vector3d_scalar_mul(eye_right_202X, (8.0 / v_204X));
  v_206X = (float)(600);
  dy_207X = vector3d_scalar_mul(eye_up_203X, (6.0 / v_206X));
  merged_arg3K0 = eye_right_202X;
  vector3d_inverse_return_tag = 0;
  goto vector3d_inverse;
 vector3d_inverse_return_0:
  v_208X = vector3d_inverse0_return_value;
  v1_209X = vector3d_scalar_mul(v_208X, 4.0);
  merged_arg3K0 = eye_up_203X;
  vector3d_inverse_return_tag = 1;
  goto vector3d_inverse;
 vector3d_inverse_return_1:
  v_210X = vector3d_inverse0_return_value;
  v2_211X = vector3d_scalar_mul(v_210X, 3.0);
  corner_212X = vector3d_op(v1_209X, v2_211X, HflA12);
  arg1K0 = 0;
  goto L2412;}
 L1056: {
  color_213X = arg3K0;
  sphere_214X = (struct sphere*)malloc(sizeof(struct sphere));
  if ((NULL == sphere_214X)) {
    v_215X = (struct object*)(sphere_214X);
    arg2K0 = v_215X;
    goto L1064;}
  else {
    sphere_214X->type = 1;
    sphere_214X->color = color_213X;
    sphere_214X->position = position_199X;
    sphere_214X->radius = 5.0;
    v_216X = (struct object*)(sphere_214X);
    arg2K0 = v_216X;
    goto L1064;}}
 L2412: {
  n_217X = arg1K0;
  if ((n_217X < 480000)) {
    x_218X = n_217X % 800;
    y_219X = n_217X / 800;
    v_220X = (float)(x_218X);
    v1_221X = vector3d_scalar_mul(dx_205X, v_220X);
    v_222X = (float)(y_219X);
    v2_223X = vector3d_scalar_mul(dy_207X, v_222X);
    point_224X = vector3d_op(v1_221X, v2_223X, HflA12);
    v1_225X = vector3d_op(corner_212X, point_224X, HflA12);
    v1_226X = vector3d_op(v1_225X, point_224X, Hfl_21);
    v_227X = vector3d_dot(v1_226X, v1_226X);
    v_228X = sqrt(v_227X);
    view_229X = vector3d_scalar_mul(v1_226X, (0.99999999 / v_228X));
    color_230X = shoot_ray(point_224X, view_229X, 0);
    merged_arg0K0 = (color_230X->x);
    wD1_return_tag = 0;
    goto wD1;
   wD1_return_0:
    merged_arg0K0 = (color_230X->y);
    wD1_return_tag = 1;
    goto wD1;
   wD1_return_1:
    merged_arg0K0 = (color_230X->z);
    wD1_return_tag = 2;
    goto wD1;
   wD1_return_2:
    arg1K0 = (1 + n_217X);
    goto L2412;}
  else {
    return 0;}}
 L1064: {
  v_231X = arg2K0;
  *(objects_168X + (2 + i_185X)) = v_231X;
  arg1K0 = (1 + i_185X);
  goto L1053;}
 wD1: {
  c_157X = merged_arg0K0;{
  if ((c_157X < 0.0)) {
    arg0K0 = 0.0;
    goto L696;}
  else {
    if ((1.0 < c_157X)) {
      arg0K0 = 1.0;
      goto L696;}
    else {
      arg0K0 = c_157X;
      goto L696;}}}
 L696: {
  f_232X = arg0K0;
  v_233X = (unsigned char)((f_232X * 255.0));
  write_byte(v_233X, port_189X);
  goto wD1_return;}
 wD1_return:
  switch (wD1_return_tag) {
  case 0: goto wD1_return_0;
  case 1: goto wD1_return_1;
  default: goto wD1_return_2;
  }}

 vector3d_inverse: {
  v1_156X = merged_arg3K0;{
  v_234X = v1_156X->y;
  v_235X = v1_156X->x;
  v_236X = v1_156X->z;
  vector3d_237X = (struct vector3d*)malloc(sizeof(struct vector3d));
  if ((NULL == vector3d_237X)) {
    vector3d_inverse0_return_value = vector3d_237X;
    goto vector3d_inverse_return;}
  else {
    vector3d_237X->x = (0.0 - v_235X);
    vector3d_237X->y = (0.0 - v_234X);
    vector3d_237X->z = (0.0 - v_236X);
    vector3d_inverse0_return_value = vector3d_237X;
    goto vector3d_inverse_return;}}
 vector3d_inverse_return:
  switch (vector3d_inverse_return_tag) {
  case 0: goto vector3d_inverse_return_0;
  default: goto vector3d_inverse_return_1;
  }}

}void
prescheme_init(void)
{
ambient = NULL;
point = NULL;
eye = NULL;
default_color = NULL;
scene = NULL;
scene_size = 0;
}
