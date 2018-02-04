//
//  main.cpp
//  FbxLoader
//
//  Created by Tomoya Fujii on 2017/11/16.
//  Copyright © 2017年 TomoyaFujii. All rights reserved.
//

#include "BufferObject.h"
#include "Shader.h"
#include "Transform.h"
#include "GLFWEW.h"
#include "BufferObject.h"
#include "UniformBuffer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fbxsdk.h>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "FbxLoader/FbxLoader.h"

#define FBX_DATA    "/Users/tomoyafujii/Desktop/Toroid/Toroid.fbx"
#define FBX_DATA2   "/Users/tomoyafujii/Desktop/testCube.fbx"
#define FBX_DATA3   "/Users/tomoyafujii/Downloads/Alicia/FBX/Alicia_solid_Unity.FBX"
#define FBX_DATA4   "/Users/tomoyafujii/Downloads/Wolf Rigged and Game Ready/Wolf_fbx.fbx"
#define FBX_DATA5   "/Users/tomoyafujii/Downloads/Wolf Rigged and Game Ready/Wolf_with_Animations.fbx"
#define FBX_DATA6   "/Users/tomoyafujii/Downloads/Black Dragon NEW/Dragon_Baked_Actions_fbx_6.1_ASCII.fbx"
#define FBX_DATA7   "/Users/tomoyafujii/Downloads/cqfamzdd6ww0-StickFigurea/StickFigurea.FBX"
#define FBX_DATA8   "/Users/tomoyafujii/Downloads/Low-Poly Spider/Spider.fbx"
#define FBX_DATA9   "/Users/tomoyafujii/Desktop/Model/glab.fbx"
#define FBX_DATA10 "/Users/tomoyafujii/Desktop/Model/chara2.fbx"
#define FBX_DATA11 "/Users/tomoyafujii/Desktop/Model/chara1.fbx"


#define NEW_LINE    std::cout << std::endl;
using namespace fbxsdk;
using namespace myTools;
using namespace TFEngine;


struct Vertex {
    Vector3 position;
    Vector4 color = Vector4(1,1,1,1);
    Vector2 texCoord;
    Vector3 normal;
    Vector4 tangent;
    GLuint boneIndex[4] = {0,0,0,0};
    Vector4 weights = {0.0,0.0,0.0,0.0};
};

struct AnimData{
    Quaternion quat;
    Vector3 translation;
};

//キーフレーム主導
struct Animation{
    std::string name;
    struct Keyframe{
        float time;
        //[boneIndex]
        //mapにしても良いかも
        std::vector<AnimData> animDatas;
    };
    std::vector<Keyframe> keyframes;
};

//ボーン主導
struct BoneBaseAnimation{
    std::string animName;
    struct Keyframe{
        float time;
        AnimData data;
    };
    struct Bone{
        //id と 名前はなくてもいい
        int boneId;
        std::string name;
        std::vector<Keyframe> keyframes;
    };
    std::vector<Bone> bones;
};

template<typename Ty>
struct IsHave{
    bool isHave = false;
    Ty item;
    void SetItem(const Ty& item){
        this->item = item;
        isHave = true;
    }
    bool Duplicate(const Ty& target){
        if(!isHave){
            return false;
        }
        if(item == target){
            return false;
        }
        return true;
    }
};

struct TempBoneAnimFrameData{
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
};

struct TempBoneAnimData{
    std::map<int,TempBoneAnimFrameData> frameDatas;
};

struct TempBoneData{
    std::map<FbxTime,Vector3> posData;
    std::map<FbxTime,Vector3> rotData;
    std::map<FbxTime,Vector3> scaData;
};

struct TempFbxAnimData{
    std::string animName;
    std::map<std::string, TempBoneData> bonesAnimDatas;
    std::vector<FbxTime> keyframeTimes;
};

struct VertexData{
    Matrix4x4 matMVP;
};


struct Relation{
    /**
     * インデックスバッファの値
     */
    std::vector<int> relatedIndex;
};

//各Boneのデータ
struct BoneData{
    std::string name;
    int boneId = -1;
    int parentId = -1;
    std::vector<int> children;
    Matrix4x4 baseInv;
};

struct BoneTreeData{
    std::vector<BoneData> data;
    BoneData* FindBone(std::string name){
        for(auto itr = data.begin(); itr != data.end(); ++itr){
            if(itr->name == name){
                return &(*itr);
            }
        }
        return nullptr;
    }
    
    BoneData* FindBone(int boneId){
        for(auto itr = data.begin(); itr != data.end(); ++itr){
            if(itr->boneId == boneId){
                return &(*itr);
            }
        }
        return nullptr;
    }
};

bool boneBaseGetFromLink = false;

BoneTreeData publicBoneTree;

std::vector<Vertex> vertVector;
std::vector<GLuint> indexVector;

//ボーンの名前保存
std::map<std::string, std::vector<TempBoneAnimData>> bones;

struct BonesAnim{
    std::vector<std::pair<std::string, TempBoneAnimData>> bonesVec;
    TempBoneAnimData& Find(std::string boneName){
        for(auto& bone : bonesVec){
            if(bone.first == boneName){
                return bone.second;
            }
        }
        TempBoneAnimData dummy;
        return dummy;
    }

};

BonesAnim bonesAnim;

int numTabs = 0;
void PrintTabs(){
    for(int i = 0; i < numTabs; ++i){
        printf("\t");
    }
}

Matrix4x4 toMyMat(const FbxAMatrix& fbxmat){
    Matrix4x4 ret;
    for(int row = 0; row < 4; ++row){
        for(int col = 0; col < 4; ++col){
            ret[row][col] = fbxmat.Get(row, col);
        }
    }
    return ret;
}

FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
    switch(type) {
        case FbxNodeAttribute::eUnknown: return "unidentified";
        case FbxNodeAttribute::eNull: return "null";
        case FbxNodeAttribute::eMarker: return "marker";
        case FbxNodeAttribute::eSkeleton: return "skeleton";
        case FbxNodeAttribute::eMesh: return "mesh";
        case FbxNodeAttribute::eNurbs: return "nurbs";
        case FbxNodeAttribute::ePatch: return "patch";
        case FbxNodeAttribute::eCamera: return "camera";
        case FbxNodeAttribute::eCameraStereo: return "stereo";
        case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
        case FbxNodeAttribute::eLight: return "light";
        case FbxNodeAttribute::eOpticalReference: return "optical reference";
        case FbxNodeAttribute::eOpticalMarker: return "marker";
        case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
        case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
        case FbxNodeAttribute::eBoundary: return "boundary";
        case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
        case FbxNodeAttribute::eShape: return "shape";
        case FbxNodeAttribute::eLODGroup: return "lodgroup";
        case FbxNodeAttribute::eSubDiv: return "subdiv";
        default: return "unknown";
    }
}

void PrintAttribute(FbxNodeAttribute* pAttribute){
    if(!pAttribute){
        return;
    }
    
    FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
    FbxString attrName = pAttribute->GetName();
    PrintTabs();
    printf("%s : %s\n", typeName.Buffer(), attrName.Buffer());
}


template <typename T>
T GetElement
(
 FbxGeometryElement::EMappingMode mappingMode,
 bool isDirectRef,
 const FbxLayerElementArrayTemplate<int>* pIndexList,
 const FbxLayerElementArrayTemplate<T>* pList,
 int cpIndex,
 int polygonVertx,
 const T& defaultValue
){
    switch (mappingMode) {
        case fbxsdk::FbxLayerElement::eByControlPoint:
            return (*pList)[isDirectRef ? cpIndex : (*pIndexList)[cpIndex]];
        case fbxsdk::FbxLayerElement::eByPolygonVertex:
            return (*pList)[isDirectRef ? polygonVertx : (*pIndexList)[polygonVertx]];
        default:
            return defaultValue;
    }
}

struct TempMaterial{
    std::string name;
    std::vector<int> indeces;
    std::vector<Vertex> verteces;
    std::vector<std::string> textureName;
};

struct Mesh{
    std::string name;
    std::vector<TempMaterial> materials;
};

std::vector<std::vector<TempMaterial>> meshes;

std::vector<Matrix4x4> baseInvs;

int polycount = 0;

//void LoadMesh(FbxMesh* mesh){
//
//    auto toVector4 = [](auto v){
//        return Vector4(v[0],v[1],v[2],v[3]);
//    };
//
//    auto toVector3 = [](auto v){
//        return Vector3(v[0],v[1],v[2]);
//    };
//
//    auto toVector2 = [](FbxVector2 v){
//        return Vector2(v[0],v[1]);
//    };
//
//    //頂点数
//    int cpCount = mesh->GetControlPointsCount();
//    //頂点配列
//    FbxVector4* controlPoints = mesh->GetControlPoints();
//
//    //インデックスバッファ数
//    int indexCount = mesh->GetPolygonVertexCount();
//    //インデックスバッファ配列
//    int* index = mesh->GetPolygonVertices();
//
//    int skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
//
//    //コントロールポイント毎のウェイト取得
//    struct perCpBoneIndexAndWeight{
//        std::vector<std::pair<int, double>> weights;
//    };
//
//    std::vector<perCpBoneIndexAndWeight> cpWeights(cpCount);
//    if(skinCount){
//        for(int i = 0; i < skinCount; ++i){
//            FbxSkin* skin = static_cast<FbxSkin*>(mesh->GetDeformer(i, FbxDeformer::eSkin));
//            if(skin){
//                int clusterCount = skin->GetClusterCount();
//                bonesAnim.bonesVec.reserve(clusterCount);
//
//                for(int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex){
//                    FbxCluster* cluster = skin->GetCluster(clusterIndex);
////                    switch (cluster->GetLinkMode()) {
////                        case fbxsdk::FbxCluster::eAdditive:
////                            std::cout << "additive" << std::endl;
////                            break;
////                        case fbxsdk::FbxCluster::eNormalize:
////                            std::cout << "normalize" << std::endl;
////                            break;
////                        default:
////                            std::cout << "total1" << std::endl;
////                            break;
////                    }
//
//                    BoneData* pData = publicBoneTree.FindBone(cluster->GetLink()->GetName());
//
//                    //影響を与える頂点インデックス(ControlPointのIndex)とそのWeightの取得
//                    int relatedCpCount = cluster->GetControlPointIndicesCount();
//                    int* relatedCpIndex = cluster->GetControlPointIndices();
//                    double* weights = cluster->GetControlPointWeights();
//                    for(int r = 0; r < relatedCpCount; ++r){
//                        std::vector<std::pair<int, double>>& pair = cpWeights[relatedCpIndex[r]].weights;
//
//                        cpWeights[relatedCpIndex[r]].weights.push_back({pData ? pData->boneId : clusterIndex, weights[r]});
//                    }
//                }
//            }
//        }
//
//        //ウェイトを４つに制限とウェイトを正規化
//        for(auto& w : cpWeights){
//            double sum = 0;
//            int count = 0;
//            if(w.weights.size() > 4){
//                std::sort(w.weights.begin(), w.weights.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b){ return a.second > b.second;});
//                w.weights.erase(w.weights.begin() + 4, w.weights.end());
//            }
//
//            for(auto& weight : w.weights){
//                sum += weight.second;
//                count++;
//            }
//            for(auto& weight : w.weights){
//                weight.second /= sum;
//            }
//        }
//    }
//
//    FbxNode* meshNode = mesh->GetNode();
//
//    FbxAMatrix mat = meshNode->EvaluateGlobalTransform();
//    FbxAMatrix rot(FbxVector4(0, 0, 0), mat.GetR(), FbxVector4(1,1,1));
//
//
//    int materialCount = mesh->GetNode()->GetMaterialCount();
//    std::cout << "material num : " << materialCount << std::endl;
//    materialCount = materialCount ? materialCount : 1;
//
//    std::vector<TempMaterial> materials(materialCount);
//
//    for(int i = 0; i < materialCount; ++i){
//        FbxSurfaceMaterial* material = meshNode->GetMaterial(i);
//        if(material){
//            materials[i].name = material->GetName();
//            FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
//            int layerNum = property.GetSrcObjectCount<FbxLayeredTexture>();
//            if(0 < layerNum){
//                for(int j = 0; j < layerNum; ++j){
//                    FbxLayeredTexture* layeredTexture = property.GetSrcObject<FbxLayeredTexture>(j);
//                    int textureCount = layeredTexture->GetSrcObjectCount<FbxFileTexture>();
//                    for(int textureIndex = 0; textureIndex < textureCount; ++textureIndex){
//                        FbxFileTexture* texture = layeredTexture->GetSrcObject<FbxFileTexture>(textureIndex);
//                        if(texture){
//                            std::string textureName = texture->GetRelativeFileName();
//                            std::cout << textureName << std::endl;
//                            materials[i].textureName.push_back(textureName);
//                        }
//                    }
//                }
//            }
//            else {
//                int fileTextureCount = property.GetSrcObjectCount<FbxFileTexture>();
//                if(0 < fileTextureCount){
//                    for(int j = 0; j < fileTextureCount; ++j){
//                        FbxFileTexture* texture = property.GetSrcObject<FbxFileTexture>(j);
//                        if(texture){
//                            std::string textureName = texture->GetRelativeFileName();
//                            std::cout << textureName << std::endl;
//                            materials[i].textureName.push_back(textureName);
//                            std::string UVSetName = texture->UVSet.Get().Buffer();
//                            std::cout << UVSetName << std::endl;
//                        }
//                    }
//                }
//            }
//        }
//    }
//
//    const bool hasColor = mesh->GetElementVertexColorCount() > 0;
//    const bool hasNormal = mesh->GetElementNormalCount() > 0;
//    const bool hasTexCoord = mesh->GetElementUVCount() > 0;
//    const bool hasTangent = mesh->GetElementTangentCount() > 0;
//
//    FbxStringList uvSetNameList;
//    mesh->GetUVSetNames(uvSetNameList);
//
//    FbxGeometryElement::EMappingMode colorMappingMode = FbxGeometryElement::EMappingMode::eNone;
//    bool isColorDirectRef = true;
//    const FbxLayerElementArrayTemplate<int>* colorIndexList = nullptr;
//    const FbxLayerElementArrayTemplate<FbxColor>* colorList = nullptr;
//    if(hasColor){
//        const FbxGeometryElementVertexColor* fbxColorList = mesh->GetElementVertexColor();
//        colorMappingMode = fbxColorList->GetMappingMode();
//        isColorDirectRef = fbxColorList->GetReferenceMode() == FbxLayerElement::eDirect;
//        colorIndexList = &fbxColorList->GetIndexArray();
//        colorList = &fbxColorList->GetDirectArray();
//    }
//
//    FbxGeometryElement::EMappingMode tangentMappingMode = FbxGeometryElement::EMappingMode::eNone;
//    bool isTangentDirectRef = true;
//    const FbxLayerElementArrayTemplate<int>* tangentIndexList = nullptr;
//    const FbxLayerElementArrayTemplate<FbxVector4>* tangentList = nullptr;
//    FbxGeometryElement::EMappingMode binormalMappingMode = FbxGeometryElement::EMappingMode::eNone;
//    bool isBinormalDirectRef = true;
//    const FbxLayerElementArrayTemplate<int>* binormalIndexList = nullptr;
//    const FbxLayerElementArrayTemplate<FbxVector4>* binormalList = nullptr;
//    if(hasTangent){
//        const FbxGeometryElementTangent* fbxTangentList = mesh->GetElementTangent();
//        tangentMappingMode = fbxTangentList->GetMappingMode();
//        isTangentDirectRef = fbxTangentList->GetReferenceMode() == FbxLayerElement::eDirect;
//        tangentIndexList = &fbxTangentList->GetIndexArray();
//        tangentList = &fbxTangentList->GetDirectArray();
//        const FbxGeometryElementBinormal* fbxBinormalList = mesh->GetElementBinormal();
//        binormalMappingMode = fbxBinormalList->GetMappingMode();
//        isBinormalDirectRef = fbxBinormalList->GetReferenceMode() == FbxLayerElement::eDirect;
//        binormalIndexList = &fbxBinormalList->GetIndexArray();
//        binormalList = &fbxBinormalList->GetDirectArray();
//    }
//
//    const FbxLayerElementArrayTemplate<int>* materialIndexList = nullptr;
//    if(FbxGeometryElementMaterial* fbxMaterialLayer = mesh->GetElementMaterial()){
//        materialIndexList = &fbxMaterialLayer->GetIndexArray();
//    }
//
//    //[マテリアルカウント][cpIndex]
//    std::vector<std::vector<Relation>> relations(materialCount);
//    for(auto& rel : relations){
//        rel.resize(cpCount);
//    }
//
//    const int polygonCount = mesh->GetPolygonCount();
//    polycount += polygonCount;
//
//    int polygonVertex = 0;
//    for(int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex){
//        for(int pos = 0; pos < 3; ++pos){
//
//            const int cpIndex = mesh->GetPolygonVertex(polygonIndex, pos);
//            Vertex v;
//            v.position = toVector3(mat.MultT(controlPoints[cpIndex]));
//            static float buf = 10.0f;
//            static int cou = 0;
//            cou++;
//
//            if(hasColor){
//                v.color  = toVector4(GetElement(colorMappingMode, isColorDirectRef, colorIndexList, colorList, cpIndex, polygonVertex,FbxColor(1,1,1,1)));
//            }
//            if(hasTexCoord){
//                FbxVector2 uv;
//                bool unmapped;
//                mesh->GetPolygonVertexUV(polygonIndex, pos, uvSetNameList[0], uv, unmapped);
//                v.texCoord = toVector2(uv);
//            }
//
//            if(hasNormal){
//                FbxVector4 norm;
//                mesh->GetPolygonVertexNormal(polygonIndex, pos, norm);
//                v.normal = toVector3(rot.MultT(norm));
//            }
//
//            v.color = Vector4((Vector3(1,1,1) + v.normal) * 0.5f);
//
//            for(int boneIndex = 0; boneIndex < cpWeights[cpIndex].weights.size(); ++boneIndex){
//                v.boneIndex[boneIndex] = cpWeights[cpIndex].weights[boneIndex].first;
//                v.weights[boneIndex] = cpWeights[cpIndex].weights[boneIndex].second;
//            }
//
//            static int add = 0;
//
//            //ポリゴンの所属するマテリアルのインデックスの取得
//            int materialIndex = materialIndexList ? (*materialIndexList)[polygonIndex] : 0;
//            TempMaterial& materialData = materials[materialIndex];
//
//            int size = relations[materialIndex][cpIndex].relatedIndex.size();
//            if(size == 0){
//                int pushIndex = materialData.verteces.size();
//                materialData.indeces.push_back(pushIndex);
//                materialData.verteces.push_back(v);
//                relations[materialIndex][cpIndex].relatedIndex.push_back(pushIndex);
//            }
//            else {
//                bool isSame = false;
//                auto itr = relations[materialIndex][cpIndex].relatedIndex.begin();
//                for(;itr != relations[materialIndex][cpIndex].relatedIndex.end(); ++itr){
//                    Vertex& tmp = materialData.verteces[*itr];
//                    if(tmp.color == v.color &&
//                       tmp.texCoord == v.texCoord &&
//                       tmp.normal == v.normal){
//                        isSame = true;
//                        break;
//                    }
//                }
//                if(isSame){
//                    materialData.indeces.push_back(*itr);
//                }
//                else {
//                    int pushIndex = materialData.verteces.size();
//                    materialData.indeces.push_back(pushIndex);
//                    materialData.verteces.push_back(v);
//                    relations[materialIndex][cpIndex].relatedIndex.push_back(pushIndex);
//                }
//            }
//        }
//    }
//
//    meshes.push_back(materials);
//}

void PrintNode(FbxNode* pNode){
    PrintTabs();
    const char* nodeName = pNode->GetName();
    FbxDouble3 translation = pNode->LclTranslation.Get();
    FbxDouble3 rotation = pNode->LclRotation.Get();
    FbxDouble3 scaling = pNode->LclScaling.Get();
    
    printf(" name : %s\n",nodeName);
    numTabs++;
    FbxNodeAttribute::EType attributeType;
    FbxNodeAttribute* attribute;
    for(int i = 0; i < pNode->GetNodeAttributeCount(); ++i){
        PrintAttribute(pNode->GetNodeAttributeByIndex(i));
        attribute = pNode->GetNodeAttributeByIndex(i);
        attributeType = attribute->GetAttributeType();
        switch (attributeType) {
            case fbxsdk::FbxNodeAttribute::eMesh:
//                LoadMesh(dynamic_cast<FbxMesh*>(attribute));
                break;
                
            default:
                break;
        }
        
    }
    
    for(int i = 0; i < pNode->GetChildCount(); ++i){
        PrintNode(pNode->GetChild(i));
    }
    
    numTabs--;
    PrintTabs();
    printf("\n");
}

void ErrorCallback(int error, const char* desc){
    std::cerr << "ERROR : " << desc << std::endl;
}

void LoadAnimCurve(FbxAnimLayer* animLayer, FbxNode* pNode, TempBoneData& boneData){
    FbxAnimCurve* animCurve;
    auto GetAnimCurveValue = [&](auto& data, auto type, std::function<void(FbxTime,float)> loopBody){
        animCurve = data.GetCurve(animLayer, type);
        if(animCurve){
            int count = animCurve->KeyGetCount();
            for(int i = 0; i < count; ++i){
                if(loopBody){
                    loopBody(animCurve->KeyGetTime(i),animCurve->KeyGetValue(i));
                }
            }
        }
    };
    
    //Translation : X
    GetAnimCurveValue(pNode->LclTranslation, FBXSDK_CURVENODE_COMPONENT_X,
                      [&](FbxTime time, float value){
                          boneData.posData[time].x = value;
                      });
    //Translation : Y
    GetAnimCurveValue(pNode->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Y,
                      [&](FbxTime time, float value){
                          boneData.posData[time].y = value;
                      });
    //Translation : Z
    GetAnimCurveValue(pNode->LclTranslation, FBXSDK_CURVENODE_COMPONENT_Z,
                      [&](FbxTime time, float value){
                          boneData.posData[time].z = value;
                      });
    
    //Rotation : X
    GetAnimCurveValue(pNode->LclRotation, FBXSDK_CURVENODE_COMPONENT_X,
                      [&](FbxTime time, float value){
                          boneData.rotData[time].x = value;
                      });
    
    //Rotation : Y
    GetAnimCurveValue(pNode->LclRotation, FBXSDK_CURVENODE_COMPONENT_Y,
                      [&](FbxTime time, float value){
                          boneData.rotData[time].y = value;
                      });
    
    //Rotation : Z
    GetAnimCurveValue(pNode->LclRotation, FBXSDK_CURVENODE_COMPONENT_Z,
                      [&](FbxTime time, float value){
                          boneData.rotData[time].z = value;
                      });
    
}

void LoadBoneCurve(FbxAnimLayer* animLayer, FbxNode* pNode, TempFbxAnimData& data){
    std::string nodeName = pNode->GetName();
    auto itr = bones.find(nodeName);
    if(itr != bones.end()){
        LoadAnimCurve(animLayer, pNode, data.bonesAnimDatas[nodeName]);
        std::map<FbxTime,float> times;
        auto tempfunc = [&](auto& data){
            for(auto itr = data.begin(); itr != data.end(); ++itr){
                times[itr->first] = static_cast<float>(itr->first.GetSecondDouble());
            }
        };
        tempfunc(data.bonesAnimDatas[nodeName].posData);
        tempfunc(data.bonesAnimDatas[nodeName].rotData);
        tempfunc(data.bonesAnimDatas[nodeName].scaData);
        for(auto itr = times.begin(); itr != times.end(); ++itr){
            data.keyframeTimes.push_back(itr->first);
        }
    }
    
    for(int i = 0; i < pNode->GetChildCount(); ++i){
        LoadBoneCurve(animLayer, pNode->GetChild(i), data);
    }
    
    auto& keytimes = data.keyframeTimes;
    std::sort(keytimes.begin(), keytimes.end());
    auto tmpitr = std::unique(keytimes.begin(), keytimes.end());
    keytimes.erase(tmpitr, keytimes.end());
}

//skinから初期姿勢を取得する
FbxSkin* FindSkin(FbxNode* node){
    FbxMesh* mesh = node->GetMesh();
    if(mesh){
        if(mesh->GetDeformerCount(FbxDeformer::eSkin)){
            FbxSkin* skin = static_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin));
            if(skin){
                return skin;
            }
        }
    }
    int childCount = node->GetChildCount();
    for(int i = 0; i < childCount ; ++i){
        FbxSkin* skin = FindSkin(node->GetChild(i));
        if(skin){
            return skin;
        }
    }
    return nullptr;
}

//キーフレーム時の姿勢を取得する用
FbxNode* FindRootBone(FbxNode* node){
    int attrCount = node->GetNodeAttributeCount();
    for(int i = 0; i < attrCount; ++i){
        FbxNodeAttribute* attr = node->GetNodeAttributeByIndex(i);
        if(attr && attr->GetAttributeType() == FbxNodeAttribute::eSkeleton){
            FbxSkeleton* skeleton = static_cast<FbxSkeleton*>(attr);
            if(skeleton->IsSkeletonRoot()){
                return node;
            }
        }
    }
    
    int childCount = node->GetChildCount();
    for(int i = 0; i < childCount; ++i){
        FbxNode* ret = FindRootBone(node->GetChild(i));
        if(ret){
            return ret;
        }
    }
    return nullptr;
}




int childSum = 0;

void AddChildren(BoneTreeData& boneTree, FbxNode* node){
    std::cout << "* AddChildren * " << std::endl;
    std::cout << "node name : " << node->GetName() << std::endl;
    BoneData* pData = boneTree.FindBone(node->GetName());
    if(!pData){
        return;
    }
    int childCount = node->GetChildCount();
    childSum += childCount;
    std::cout << "child count : " << childCount << std::endl;
    pData->children.reserve(childCount);
    for(int i = 0 ; i < childCount; ++i){
        std::cout << "child name : " << node->GetChild(i)->GetName() << std::endl;
    }
    for(int i = 0;i < childCount; ++i){
        FbxNode* child = node->GetChild(i);
        if(child->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eSkeleton){
            continue;
        }
        BoneData* tmp = boneTree.FindBone(child->GetName());
        if(tmp){
            pData->children.push_back(tmp->boneId);
//            AddChildren(boneTree, child);
        }
    }
    NEW_LINE;
}


//ボーン構造取得
BoneTreeData GetBoneTreeData(const FbxScene* scene){
    FbxSkin* skin = FindSkin(scene->GetRootNode());
    FbxNode* rootBone = FindRootBone(scene->GetRootNode());
    if(!skin || !rootBone){
        return {};
    }
    
    int clusterCount = skin->GetClusterCount();
    std::cout << "bone tree cluster count : " << clusterCount << std::endl;
    BoneTreeData boneTree;
    boneTree.data.reserve(clusterCount);
    BoneData data;
    for(int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex){
        FbxCluster* cluster = skin->GetCluster(clusterIndex);
        FbxAMatrix transform;
        cluster->GetTransformLinkMatrix(transform);
        data.boneId = clusterIndex;
        data.name = cluster->GetLink()->GetName();
        data.baseInv = toMyMat(transform);
        boneTree.data.push_back(data);
        std::cout << "cluster name : " << data.name << std::endl;
    }
    for(int i = 0; i < clusterCount; ++i){
        AddChildren(boneTree, skin->GetCluster(i)->GetLink());
    }
//    AddChildren(boneTree, rootBone);
    return boneTree;
}

FbxMesh* FindIncludedMesh(FbxScene* scene, FbxCluster* cluster){
    int meshCount = scene->GetSrcObjectCount<FbxMesh>();
    for (int i = 0; i < meshCount; ++i) {
        FbxMesh* mesh = scene->GetSrcObject<FbxMesh>(i);
        if(!mesh->GetDeformerCount(FbxDeformer::eSkin)){
            continue;
        }
    
        FbxSkin* skin = static_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin));
        int clusterCount = skin->GetClusterCount();
        for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex) {
            FbxCluster* meshCluster = skin->GetCluster(clusterIndex);
            if(meshCluster == cluster){
                return mesh;
            }
        }
    }
    return nullptr;
}

//BoneのIndexを階層構造から決定する
//Weight設定時のインデックスはクラスターのリンクの名前でTreeDataからデータを検索してそこからindexを取得すること
BoneTreeData LoadBone(FbxScene* scene){
    BoneTreeData boneTree;
    FbxNode* rootBone = FindRootBone(scene->GetRootNode());
    if(!rootBone){
        return {};
    }
    struct GetSkeleton{
        static int Run(FbxNode* node, BoneTreeData& boneTree, int parentId){
            if(node->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eSkeleton){
                return -1;
            }
            BoneData boneData;
            
            boneData.boneId = boneTree.data.size();
            boneData.parentId = parentId;
            boneData.name = node->GetName();
            
            boneTree.data.push_back(boneData);
            
            int childCount = node->GetChildCount();
            for(int i = 0; i < childCount; ++i){
                boneData.children.push_back(Run(node->GetChild(i),boneTree,boneData.boneId));
            }
            return boneData.boneId;
        }
    };
    
    auto GetGeometry = [](FbxNode* pNode)->FbxAMatrix
    {
        const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
        const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
        const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
        
        return FbxAMatrix(lT, lR, lS);
    };
    
    GetSkeleton::Run(rootBone, boneTree, -1);
    int clusterCount = scene->GetSrcObjectCount<FbxCluster>();
    for(int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex){
        FbxCluster* cluster = scene->GetSrcObject<FbxCluster>(clusterIndex);
        const FbxMesh* includedMesh = FindIncludedMesh(scene, cluster);
        if(includedMesh && !boneBaseGetFromLink){
            FbxAMatrix a,b,c;
            cluster->GetTransformLinkMatrix(a);
            cluster->GetTransformMatrix(b);
            c = GetGeometry(includedMesh->GetNode());
            BoneData* pData = boneTree.FindBone(cluster->GetLink()->GetName());
            if(pData){
                pData->baseInv = toMyMat(a.Inverse() * b * c);
            }
        }
        else {
            BoneData* pData = boneTree.FindBone(cluster->GetLink()->GetName());
            if(pData){
                FbxAMatrix tmp;
                cluster->GetTransformLinkMatrix(tmp);
                pData->baseInv = toMyMat(tmp.Inverse());
            }
        }
    }
    return boneTree;
}

void ShowPose(FbxScene*& scene){
    std::cout << "Dispalay Pose" << std::endl;
    int poseCount = scene->GetPoseCount();
    std::cout << "Pose Count : " << poseCount << std::endl;
    for(int i = 0 ; i < poseCount; ++i){
        FbxPose* pose = scene->GetPose(i);
        std::cout << "Pose" << i << " name : " << pose->GetName() << std::endl;
        std::cout << "Is Bind Pose : " << pose->IsBindPose() << std::endl;
        std::cout << "Numver of items in the pose : " << pose->GetCount() << std::endl;
        for(int j = 0; j < pose->GetCount(); ++j){
            std::cout << "Item name : " << pose->GetNode(j)->GetName() << std::endl;
        }
    }
}

struct BoneAnimation{
    BoneTreeData boneTree;
    std::map<std::string,Animation> animations;
};

struct BoneBAnimation{
    BoneTreeData boneTree;
    //first : アニメーション名  second : ボーンベースデータ
    std::map<std::string, BoneBaseAnimation> animations;
};

struct Test{
    typedef std::pair<float,Matrix4x4> TimeMatPair;
    std::vector<std::pair<float, Matrix4x4>> animDatas;
    int current = -1;
    int next = -1;
    float animationTime = 0;
    Matrix4x4 GetMat(float time){
        if(!animDatas.size()){
            return {};
        }
        
        if(animDatas.size() == 1){
            return animDatas[0].second;
        }
        
        if(time > animationTime){
            while(time > animationTime){
                time -= animationTime;
            }
            current = 0;
            next = 1;
        }
        
        if(current == -1){
            for(int i = 0; i < animDatas.size(); ++i){
                if(time < animDatas[i].first){
                    next = i;
                    current = i - 1;
                    break;
                }
            }
        }
        
        if(time < animDatas[current].first){
            Reset();
        }
        
        if(current == -1){
            for(int i = 0; i < animDatas.size(); ++i){
                if(time < animDatas[i].first){
                    next = i;
                    current = i - 1;
                    break;
                }
            }
        }
        
        while(time > animDatas[next].first){
            current++;
            next++;
            if(next >= animDatas.size()){
                next = animDatas.size() - 1;
                current = next - 1;
                return animDatas.back().second;
            }
        }
        
        float duration = animDatas[next].first - animDatas[current].first;
        float rate = (time - animDatas[current].first) / duration;
        if(rate > 1 || rate < 0){
            std::cout << "irregular happened" << std::endl;
        }
        return animDatas[current].second * (1 - rate) + animDatas[next].second * rate;
    }
    
    void Reset(){
        current = -1;
        next = -1;
    }
};

struct TestAgg{
    float animationTime = 0;
    //[bone]
    std::vector<Test> tests;
    
    std::vector<Matrix4x4> GetMat(float time){
        int boneNum = tests.size();
        std::vector<Matrix4x4> mat(boneNum);
        for(int i = 0; i < boneNum; ++i){
            if(time > animationTime || time == 0.0f){
                tests[i].Reset();
                time = 0;
            }
            mat[i] = tests[i].GetMat(time);
        }
        return mat;
    }
};

std::vector<TestAgg> animes;

BoneAnimation LoadAnimation(FbxScene* scene){
    BoneAnimation anim;
    BoneBAnimation bonebase;
    //ボーンの情報の取得
    anim.boneTree = publicBoneTree;
    bonebase.boneTree = anim.boneTree;
    
    int boneCount = anim.boneTree.data.size();
    //アニメーション数の取得
    int animCount = scene->GetSrcObjectCount<FbxAnimStack>();
    if(!animCount){
        return {};
    }
    //ボーン毎にアニメーションカーブを保存用
    std::vector<TempFbxAnimData> tempAnimData(animCount);
    
    FbxSkin* skin = FindSkin(scene->GetRootNode());
    if(!skin){
        return {};
    }
    FbxNode* rootBone = FindRootBone(scene->GetRootNode());
    
    FbxNode* meshNode = nullptr;
    int meshCount = scene->GetMemberCount<FbxMesh>();
    for(int meshIndex = 0; meshIndex < meshCount; ++meshIndex){
        FbxMesh* mesh = scene->GetMember<FbxMesh>(meshIndex);
        if(mesh->GetDeformerCount(FbxDeformer::eSkin) > 0){
           meshNode = mesh->GetNode();
        }
    }
    
    FbxTime period;
    period.SetTime(0, 0, 0, 1, 0, scene->GetGlobalSettings().GetTimeMode());
    
    auto toMyQuat = [](FbxQuaternion fbxquat){
        return Quaternion(fbxquat[0],fbxquat[1],fbxquat[2],fbxquat[3]);
    };
    
    auto toVector3 = [](auto v){
        return Vector3(v[0],v[1],v[2]);
    };
    
    animes.resize(animCount);
    for(int animIndex = 0; animIndex < animCount; ++animIndex){
        FbxAnimStack* animStack = scene->GetSrcObject<FbxAnimStack>(animIndex);
        scene->SetCurrentAnimationStack(animStack);
        std::string animName = animStack->GetName();
        FbxTime start = animStack->GetLocalTimeSpan().GetStart();
        FbxTime stop = animStack->GetLocalTimeSpan().GetStop();
        int keyCount = animStack->GetLocalTimeSpan().GetDuration().Get() / period.Get();
        
        animStack->GetReferenceTimeSpan();
        
        animes[animIndex].animationTime = stop.GetSecondDouble();

        animes[animIndex].tests.resize(publicBoneTree.data.size());
        
        int clusterCount = scene->GetSrcObjectCount<FbxCluster>();
        for(int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex){
            FbxCluster* cluster = scene->GetSrcObject<FbxCluster>(clusterIndex);
            auto includeMesh = FindIncludedMesh(scene, cluster);
            if(includeMesh){
                meshNode = includeMesh->GetNode();
            }
            auto pData = publicBoneTree.FindBone(cluster->GetLink()->GetName());
            if(!pData){
                continue;
            }
            
            auto& buf = animes[animIndex].tests[pData->boneId];
            for(int keyframe = 0; keyframe <= keyCount; ++keyframe){
                FbxAMatrix mat = cluster->GetLink()->EvaluateGlobalTransform(period * keyframe);
                if(meshNode && !boneBaseGetFromLink){
                    FbxAMatrix meshMat = meshNode->EvaluateGlobalTransform(period * keyframe);
                    auto GetGeometry = [](FbxNode* pNode)->FbxAMatrix
                    {
                        const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
                        const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
                        const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
                        
                        return FbxAMatrix(lT, lR, lS);
                    };
                    mat = (meshMat * GetGeometry(meshNode)).Inverse() * mat;
                }
                buf.animDatas.push_back(
                {
                    static_cast<float>((period * (keyframe)).GetSecondDouble()),
                    toMyMat(mat)
                });
            }
            buf.animationTime = buf.animDatas.back().first;
        }
        
        
//        int clusterCount = skin->GetClusterCount();
//        for(int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex){
//            FbxCluster* cluster = skin->GetCluster(clusterIndex);
//            auto pData = publicBoneTree.FindBone(cluster->GetLink()->GetName());
//            if(!pData){
//                continue;
//            }
//            auto& buf = animes[animIndex].tests[pData->boneId];
//            for(int keyframe = 0; keyframe < keyCount; ++keyframe){
//                AnimData tmp;
//                FbxAMatrix mat = cluster->GetLink()->EvaluateGlobalTransform(period * keyframe);
//                if(meshNode && !boneBaseGetFromLink){
//                    FbxAMatrix meshMat = meshNode->EvaluateGlobalTransform(period * keyframe);
//                    auto GetGeometry = [](FbxNode* pNode)->FbxAMatrix
//                    {
//                        const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
//                        const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
//                        const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
//
//                        return FbxAMatrix(lT, lR, lS);
//                    };
//
//                    mat = (meshMat * GetGeometry(meshNode)).Inverse() * mat;
//                }
//                tmp.translation = toVector3(mat.GetT());
//                tmp.quat = toMyQuat(mat.GetQ());
//                anime.keyframes[keyframe].animDatas[publicBoneTree.FindBone(cluster->GetLink()->GetName())->boneId] = tmp;
//                anime.keyframes[keyframe].time = static_cast<float>((period * (keyframe)).GetSecondDouble());
//
//                buf.animDatas.push_back(
//                {
//                    static_cast<float>((period * (keyframe)).GetSecondDouble()),
//                    toMyMat(mat)
//                });
//            }
//            buf.animDatas.push_back(
//            {
//                static_cast<float>(stop.GetSecondDouble()),
//                toMyMat(cluster->GetLink()->EvaluateGlobalTransform(stop))
//            });
//            buf.animationTime = buf.animDatas.back().first;
//        }
//
//        FbxAnimLayer* animLayer = animStack->GetSrcObject<FbxAnimLayer>(0);
//        if(!animLayer){
//            continue;
//        }
//        TempFbxAnimData tempfbxanim;
//        LoadBoneCurve(animLayer, scene->GetRootNode(), tempfbxanim);
//
//        auto timeCount = [](auto& data, std::vector<FbxTime>& times){
//            for(auto itr = data.begin(); itr != data.end(); ++itr){
//                times.push_back(itr->first);
//            }
//        };
        
//        clusterCount = skin->GetClusterCount();
        
//        for(int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex){
//            FbxCluster* cluster = skin->GetCluster(clusterIndex);
//            //std::cout << "cluster name : " << cluster->GetName() << std::endl;
//            auto& data = tempfbxanim.bonesAnimDatas[cluster->GetLink()->GetName()];
//            std::vector<FbxTime> times;
//            timeCount(data.posData, times);
//            timeCount(data.rotData, times);
//            timeCount(data.scaData, times);
//            std::sort(times.begin(), times.end());
//
//            int timecount = times.size();
//
//            times.erase(std::unique(times.begin(), times.end()), times.end());
//
//            if(times.size() == 0 ){
//                std::cout << "timecount : " << timecount << std::endl;
//            }
//
//            auto& buf = animes[animIndex].tests[bonebase.boneTree.FindBone(cluster->GetLink()->GetName())->boneId];
//            for(auto itr = times.begin(); itr != times.end(); ++itr){
//                buf.animDatas.push_back({static_cast<float>((*itr).GetSecondDouble()),
//                    toMyMat(cluster->GetLink()->EvaluateGlobalTransform(*itr))
//                });
//            }
//            buf.animationTime = buf.animDatas.back().first;
//        }
        
        
        
//        for(int boneIndex = 0; boneIndex < boneCount; ++boneIndex){
//            std::string boneName = bonebase.boneTree.FindBone(boneIndex)->name;
//            auto& data = tempfbxanim.bonesAnimDatas[boneName];
//            std::vector<FbxTime> times;
//            timeCount(data.posData, times);
//            timeCount(data.rotData, times);
//            timeCount(data.scaData, times);
//            std::sort(times.begin(), times.end());
//            times.erase(std::unique(times.begin(), times.end()), times.end());
//            int keyCount = times.size();
//            for(int keyframe = 0; keyframe < keyCount; ++keyframe){
//
//            }
//            bonebase.animations[animName].bones[boneIndex];
//        }
        
        
//        tempAnimData[animIndex].animName = animName;
//        int animLayerCount = animStack->GetSrcObjectCount<FbxAnimLayer>();
//        if(animLayerCount){
//            LoadBoneCurve(animStack->GetSrcObject<FbxAnimLayer>(0), rootBone, tempAnimData[animIndex]);
//        }
//
//        for(int boneIndex = 0; boneIndex < boneCount; ++boneIndex){
//            auto bone = anim.boneTree.FindBone(boneIndex);
//            if(!bone){
//                std::cerr << "Bone is not found." << std::endl;
//                continue;
//            }
//            for(int keyframe = 0; keyframe < tempAnimData[animIndex].keyframeTimes.size(); ++keyframe){
//                FbxTime keyframeTime = tempAnimData[animIndex].keyframeTimes[keyframe];
//                anim.animations[animName].keyframes[keyframe].time = static_cast<float>(keyframeTime.GetSecondDouble());
//                anim.animations[animName].keyframes[keyframe].animDatas[bone->boneId] = ConvertToAnimData(tempAnimData[animIndex].bonesAnimDatas[bone->name]);
//            }
//        }
        
    }
    
    return anim;
}

struct KeyboardCallback{
    static std::function<void(int,int)> callback;
    static void Callback(GLFWwindow* window, int key, int scancode, int action, int mods){
        if(callback){
            callback(key,action);
        }
    }
};

std::function<void(int,int)> KeyboardCallback::callback = nullptr;

namespace mtLoader = myTools::FbxLoader;

int main(int argc, const char * argv[]) {
    FbxLoader::FbxLoader loader;
    loader.Initialize(FBX_DATA);
    myTools::FbxLoader::BoneTreeData boneTree;
    loader.LoadBone(boneTree);
    std::vector<myTools::FbxLoader::SkinnedMesh> skMeshes;
    std::vector<myTools::FbxLoader::StaticMesh> stMeshes;
    loader.LoadAllMesh(stMeshes,skMeshes);
    std::vector<myTools::FbxLoader::Animation> animations;
    loader.LoadAnimation(animations);
    
    GL::Window& window = GL::Window::GetInstance();
    if(!window.Init(800, 600, "title")){
        return 1;
    }
    
    GL::BufferObject vbo;
    std::vector<FbxLoader::StaticVertex> vert;
    std::vector<GLuint> indx;
    
    int matesMax = stMeshes.size();
    int materialCount = 0;
    for(int matesIndex = 0; matesIndex < matesMax; ++matesIndex){
        auto& materials = stMeshes[matesIndex].materials;
        int materialIndexMax = materials.size();
        materialCount += materialIndexMax;
        for(int materialIndex = 0; materialIndex < materialIndexMax; ++materialIndex){
            int vertSize = vert.size();
            for(int i = 0; i < materials[materialIndex].indeces.size(); ++i){
                materials[materialIndex].indeces[i] += vertSize;
            }
            indx.insert(indx.end(), materials[materialIndex].indeces.begin(), materials[materialIndex].indeces.end());
            vert.insert(vert.end(), materials[materialIndex].verteces.begin(), materials[materialIndex].verteces.end());
        }
    }
    std::cout << "material count : " << materialCount << std::endl;
    
    vbo.Init(GL_ARRAY_BUFFER, sizeof(Vertex) * vert.size(), vert.data());
    GL::BufferObject ibo;
    ibo.Init(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indx.size(), indx.data());
    int max = 0;
    for(int i = 0; i < indx.size(); ++i){
        
    }
    
    GL::VertexArrayObject vao;
    vao.Init(vbo.Id(), ibo.Id());
    vao.Bind();
//    vao.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, position));
//    vao.VertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, color));
//    vao.VertexAttribPointer(2, 4, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, boneIndex));
//    vao.VertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, weights));
    vao.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                            sizeof(mtLoader::StaticVertex), offsetof(mtLoader::StaticVertex, position));
    vao.VertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                            sizeof(mtLoader::StaticVertex), offsetof(mtLoader::StaticVertex, color));

    
    vao.Unbind();
    
    Shader::ProgramPtr shader = Shader::Program::Create
    ("ShaderCode/StaticMesh/StaticMesh.vert",
     "ShaderCode/StaticMesh/StaticMesh.frag");
    
    int boneCount = boneTree.data.size();
    baseInvs.resize(boneCount);
    for(int i = 0; i < boneCount; ++i){
        baseInvs[i] = boneTree.data[i].baseInv;
    }
    
    if(!baseInvs.size()){
        baseInvs.push_back({});
    }
    UniformBufferPtr ubo = UniformBuffer::Create(sizeof(VertexData), 0, "VertexData");
//    UniformBufferPtr boneUbo = UniformBuffer::Create(sizeof(Matrix4x4) * baseInvs.size(), 1, "BoneData");
    
    if(!vbo.Id() || !ibo.Id() || !vao.Id() || !shader){
        return 1;
    }
    
    shader->UniformBlockBinding("VertexData", 0);
//    if(!shader->UniformBlockBinding("BoneData", 1)){
//        return 1;
//    }
    glEnable(GL_DEPTH_TEST);
    
    int frame = 0;
    Matrix4x4* boneFrameRot = new Matrix4x4[baseInvs.size()];
    glEnable(GL_DEPTH_TEST);
    
    float animationTime = 0;
    float delta = 1.0f / 180.0f;
    float before = glfwGetTime();
    float current = glfwGetTime();

    static int animeIndex = 0;

    float s = 1.0f;
    
    /* キーボードコールバック */
    
    bool end = false;
    bool rotR = false;
    auto keyboardCallback = [&](int key, int action){
        if(action == GLFW_PRESS){
            return;
        }
        
        switch (key) {
            case GLFW_KEY_LEFT:
                s *= 0.5;
                break;
            case GLFW_KEY_RIGHT:
                s *= 2;
                break;
            case GLFW_KEY_ENTER:
                animeIndex++;
                if(animeIndex >= animations.size()){
                    animeIndex = 0;
                }
                break;
            case GLFW_KEY_ESCAPE:
                end = true;
                break;
            case GLFW_KEY_R:
                rotR = !rotR;
                break;
            default:
                break;
        }
    };
    
    KeyboardCallback::callback = keyboardCallback;
    glfwSetKeyCallback(window.Get(), KeyboardCallback::Callback);
    
    /* キーボードコールバック */

    Matrix4x4 r;
    
//    for(int i = 0; i < publicBoneTree.data.size(); ++i){
//        std::cout << publicBoneTree.data[i].name << std::endl;
//        std::cout << "bone matrix" << std::endl;
//        print(Inverse(publicBoneTree.data[i].baseInv));
//    }
//
    while (!window.ShouldClose() && !end) {
        
        if(rotR){
            r[0][0] = 1;
            r[1][1] = 0;
            r[1][2] =-1;
            r[2][1] = 1;
            r[2][2] = 0;
        }
        else {
            r = {};
        }
        
        before = current;
        current = glfwGetTime();
        delta = current - before;
//        delta *= 0.1f;
        glClearColor(0.1, 0.3, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader->UseProgram();
        vao.Bind();
        
        Matrix4x4 matProj = Perspective(45.0f, 600.0/ 800.0, .1f, 10000.0f);
        Matrix4x4 matView = LookAt({50,50,50}, {0,0,0}, {0,1,0});
        
        VertexData vertexData;
        Matrix4x4 scaler;
        scaler[0][0] = s;
        scaler[1][1] = s;
        scaler[2][2] = s;
        vertexData.matMVP = matProj * matView * scaler;
        
        ubo->BufferSubData(&vertexData);
        
        animationTime += delta;


        if(animations.size()){
        if(animationTime > animations[animeIndex].animationTime){
            animationTime -= animations[animeIndex].animationTime;
            animationTime = 0;

//            for(auto& data : animes[animeIndex].tests){
//                data.Reset();
//            }
        }
        }
        
//        std::cout << "time : " << animationTime << std::endl;
        
        static float rate = 0.01;
        for(int i = 0; i < baseInvs.size(); ++i){
            boneFrameRot[i] = {};
            if(animations.size()){
                if(animations[animeIndex].boneAnimationData.size()){
                    boneFrameRot[i] = animations[animeIndex].boneAnimationData[i].GetMat(animationTime) * baseInvs[i];
                }
            }
            auto mat = Translate(Vector3(0.0f,0.0f,0.0f));
            mat *= Rotate(Vector3(0,1,0), rate);
            boneFrameRot[i] = mat * r *  boneFrameRot[i];
        }

//        std::cout << animeIndex << std::endl;
        rate += 0.01;

//        boneUbo->BufferSubData(&boneFrameRot[0][0][0]);
        
        glDrawElements(GL_TRIANGLES, indx.size(), GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(0));
        window.SwapBuffers();
        frame++;
    }
    
    delete[] boneFrameRot;
    
    return 0;
}
