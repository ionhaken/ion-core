/*
 * Copyright 2015-2022 Markus Haikonen, Ionhaken
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <codegen/SourceCodeWriter.h>

namespace ion::codegen
{
struct CodegenContext;
struct StoreSettings;
class ComponentWriter : public SourceCodeWriter
{
public:
	ComponentWriter(SourceCodeWriter& other);

	void Generate(CodegenContext& context);

private:
	void GenerateHeader(uint16_t entityIndex, CodegenContext& context);
	void GenerateEntity(uint16_t entityIndex, StoreSettings& settings, bool isReadOnly);
	void GenerateFooter();
};
}  // namespace ion::codegen