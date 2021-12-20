#include "cmprs.h"
#include "draco/mesh/triangle_soup_mesh_builder.h"
#include "draco/compression/mesh/mesh_edgebreaker_encoder.h"
#include "draco/src/draco/compression/mesh/mesh_edgebreaker_decoder.h"
// #include "draco/compression/mesh/mesh_sequential_encoder.h"
#include "draco/core/encoder_buffer.h"
#include "draco/core/decoder_buffer.h"

using namespace draco;

void compress(
  float *positions,
  unsigned int numPositions,
  float *normals,
  unsigned int numNormals,
  float *colors,
  unsigned int numColors,
  uint8_t *outData,
  unsigned int *outSize
) {
  TriangleSoupMeshBuilder mb;
  mb.Start(numPositions/3);

  const int position_att_id = mb.AddAttribute(GeometryAttribute::POSITION, 3, DT_FLOAT32);
  mb.SetAttributeValuesForFace(position_att_id, FaceIndex(0),
                               Vector3f(0.f, 0.f, 0.f).data(),
                               Vector3f(1.f, 0.f, 0.f).data(),
                               Vector3f(0.f, 1.f, 0.f).data());
  const int normal_att_id = mb.AddAttribute(GeometryAttribute::NORMAL, 3, DT_FLOAT32);
  mb.SetAttributeValuesForFace(normal_att_id, FaceIndex(0),
                               Vector3f(0.f, 0.f, 0.f).data(),
                               Vector3f(1.f, 0.f, 0.f).data(),
                               Vector3f(0.f, 1.f, 0.f).data());
  const int color_att_id = mb.AddAttribute(GeometryAttribute::COLOR, 3, DT_FLOAT32);
  mb.SetAttributeValuesForFace(normal_att_id, FaceIndex(0),
                               Vector3f(0.f, 0.f, 0.f).data(),
                               Vector3f(1.f, 0.f, 0.f).data(),
                               Vector3f(0.f, 1.f, 0.f).data());

  std::unique_ptr<Mesh> mesh = mb.Finalize();

  draco::MeshEdgebreakerEncoder encoder;
  draco::EncoderBuffer buffer;

  EncoderOptions encoder_options = EncoderOptions::CreateDefaultOptions();
  // encoder_options.SetSpeed(10 - compression_level, 10 - compression_level);
  encoder.SetMesh(*mesh);

  const draco::Status status = encoder.Encode(encoder_options, &buffer);
  if (status.ok()) {
    memcpy(outData, buffer.data(), buffer.size());
    *outSize = buffer.size();
  } else {
    std::cerr << "fail" << std::endl;
    *outSize = 0;
  }
}

void decompress(
  uint8_t *data,
  unsigned int size,
  float *positions,
  unsigned int numPositions,
  float *normals,
  unsigned int numNormals,
  float *colors,
  unsigned int numColors
) {
  DecoderBuffer dec_buffer;
  dec_buffer.Init((const char *)data, size);
  MeshEdgebreakerDecoder decoder;

  std::unique_ptr<Mesh> decoded_mesh(new Mesh());
  DecoderOptions dec_options;
  const draco::Status status = decoder.Decode(dec_options, &dec_buffer, decoded_mesh.get());
  if (!status.ok()) {
    std::cerr << "fail" << std::endl;
  }
}