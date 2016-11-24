box_size = 0.010
base = 0.119
sh_yaw = 0.0581
sh_roll = 0.274125
elbow = 0.27073
wrist_yaw = 0.0415

v = {
  'box_size': box_size,
  'base': base / 2,
  'sh_yaw': sh_yaw,
  'sh_roll': sh_roll,
  'elbow': elbow,
  'wrist_yaw': wrist_yaw,

  'base_center': base,
  'sh_yaw_center': sh_yaw / 2,
  'sh_roll_center': sh_roll / 2,
  'elbow_center': elbow / 2,
  'wrist_yaw_center': wrist_yaw / 2,
}

t = {
  'base_trans': '0 0 0'.format(**v),
  'base_height': '{base:.3f}'.format(**v),  # divide by 2
  'sh_yaw_trans': '0 {base:.3f} 0'.format(**v),
  'sh_yaw_box1_trans': '0.05 {sh_yaw_center:.3f} 0'.format(**v),
  'sh_yaw_box1_extent': '{box_size:.3f} {sh_yaw:.3f} {box_size:.3f}'.format(**v),
  'sh_yaw_box2_trans': '-0.05 {sh_yaw_center:.3f} 0'.format(**v),
  'sh_yaw_box2_extent': '{box_size:.3f} {sh_yaw:.3f} {box_size:.3f}'.format(**v),
  'sh_roll_trans': '0 {sh_yaw:.3f} 0'.format(**v),
  'sh_roll_box_trans': '0 {sh_roll_center:.3f} 0'.format(**v),
  'sh_roll_box_extent': '{box_size:.3f} {sh_roll:.3f} {box_size:.3f}'.format(**v),
  'elbow_trans': '0 {sh_roll_center:.3f} 0'.format(**v),
  'elbow_box_trans': '0 {elbow_center:.3f} 0'.format(**v),
  'elbow_box_extent': '{box_size:.3f} {elbow:.3f} {box_size:.3f}'.format(**v),
  'wrist_yaw_trans': '0 {elbow_center:.3f} 0'.format(**v),
  'wrist_yaw_box_trans': '0 {wrist_yaw_center:.3f} 0'.format(**v),
  'wrist_yaw_box_extent': '{box_size:.3f} {wrist_yaw:.3f} {box_size:.3f}'.format(**v)
}

# s = open('g2.env.template.xml').read()
# f = open('g2.env.xml', 'w')
# f.write(s.format(**t))
# f.close()
