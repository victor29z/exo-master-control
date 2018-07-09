#ifndef JOINT_DATA_TYPE_H
#define JOINT_DATA_TYPE_H
typedef struct{
    unsigned int joint_pos[14];
    unsigned int joint_force[14];
    bool joint_pos_valid[14];

}JOINT_DAT_TYPE;
#endif // JOINT_DATA_TYPE_H
