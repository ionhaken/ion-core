/*
 * Copyright 2023 Markus Haikonen, Ionhaken
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
#include <ion/json/JSONDocument.h>
#if ION_EXTERNAL_JSON
	#include <ion/byte/ByteBuffer.h>
	#include <ion/byte/ByteWriter.h>
	#include <ion/core/Core.h>
	#include <ion/filesystem/File.h>
	#include <ion/filesystem/ResourceLoader.h>
	#include <ion/string/String.h>
	#include <rapidjson/encodings.h>
	#include <rapidjson/stringbuffer.h>
	#include <rapidjson/writer.h>
	#include <memory>

namespace ion
{
class JSONSerializer
{
	using StringBuffer = rapidjson::GenericStringBuffer<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<>>;

public:
	typedef char Ch;

	JSONSerializer(ByteBufferBase& buffer) : mBuffer(buffer), mWriter(nullptr) {}

	void Write(JSONDocument& document)
	{
		ByteWriter byteWriter(mBuffer);
		mWriter = &byteWriter;
		rapidjson::Writer<JSONSerializer> writer(*this);
		document.mDocument.Accept(writer);
	}

	void Put(char c) { mWriter->WriteArray(reinterpret_cast<ion::u8*>(&c), 1, 1024 * 1024); }
	void Flush() { return; }
	const uint8_t* Data() const
	{
		const uint8_t* data = nullptr;
		{
			ByteReader reader(mBuffer);
			if (reader.Available() > 0)
			{
				data = &reader.ReadAssumeAvailable<uint8_t>();
			}
		}
		return data;
	}

	size_t Size() const { return mBuffer.Size(); }

private:
	// rapidjson::MemoryPoolAllocator<> mAllocator;
	// StringBuffer mBuffer;
	ByteBufferBase& mBuffer;
	ByteWriter* mWriter;
};

JSONDocument::JSONDocument() : JSONElement(*this), mBuffer(), mAllocator(mBuffer, sizeof mBuffer), mDocument(&mAllocator, 256)
{
	mDocument.SetObject();
}

void ion::JSONDocument::Save(ByteBufferBase& buffer)
{
	JSONSerializer serializer(buffer);
	serializer.Write(*this);
}

void ion::JSONDocument::Save(const ion::Folder& folder, StringView filename)
{
	auto target = folder.FullPathTo(filename);
	Save(target);
}

void ion::JSONDocument::Save(StringView filename)
{
	ion::ByteBuffer<> buffer(32 * 1024);
	Save(buffer);
	ByteReader reader(buffer);
	ion::file_util::ReplaceTargetFile(filename, reader);
}

void ion::JSONDocument::Load(StringView target)
{
	std::string tmp = target.CStr();
	size_t pos = std::numeric_limits<size_t>::max();
	for (int i = int(tmp.length() - 1); i >= 0; i--)
	{
		if (tmp[i] == '/' || tmp[i] == '\\')
		{
			pos = static_cast<size_t>(i);
			break;
		}
	}
	if (pos < tmp.length())
	{
		auto left = tmp.substr(0, pos);
		auto right = tmp.substr(pos, tmp.size());

		ion::Folder folder(StringView(left.c_str(), left.size()));
		Load(folder, StringView(right.c_str(), right.length()), nullptr);
	}
	else
	{
		ion::Folder folder("");
		Load(folder, StringView(tmp.c_str(), tmp.length()), nullptr);
	}
}

void ion::JSONDocument::Load(ion::Folder& folder, StringView target, const ion::DataProcessorRegistry* processorregistry)
{
	ion::Vector<uint8_t> data;
	{
		FileLoader fileLoader(folder, processorregistry, target, *ion::core::gSharedScheduler);
		
		fileLoader.Wait();
		fileLoader.Get(data);
	}
	if (data.IsEmpty())
	{
		ION_ABNORMAL("Cannot read '" << target << "'");
	}	
	else
	{
		data.Add(uint8_t(0));
		Parse(target, ion::StringView(reinterpret_cast<char*>(data.Data()), data.size()-1));
	}
}

void ion::JSONDocument::Parse(StringView fileName, ion::StringView dataView)
{
	mDocument.Parse(dataView.CStr());
	if (mDocument.HasParseError())
	{
		ION_LOG_INFO("File " << fileName << " has parse error");
		size_t line = 0;
		size_t prevLineStart = 0;
		size_t lineStart = 0;
		for (size_t i = 0; i < mDocument.GetErrorOffset(); i++)
		{
			if (dataView.CStr()[i] == '\n')
			{
				prevLineStart = lineStart;
				lineStart = i + 1;
				line++;
			}
		}

		std::string data(dataView.CStr());	// #TODO: Implement substr to string view
		if (line > 1)
		{
			ION_LOG_INFO("Line " << (line) << ": '" << data.substr(prevLineStart, lineStart - prevLineStart - 1).c_str() << "'");
		}
		ION_LOG_INFO("Line " << (line + 1) << ": '" << data.substr(lineStart, mDocument.GetErrorOffset() - lineStart).c_str() << "'");
		ION_ABNORMAL("Parse error: " << int(mDocument.GetParseError()) << " ^^^");
	}
	else
	{
		mHasLoaded = true;
	}
}

JSONElement::JSONElement(JSONDocument& aDocument) : document(aDocument) {}

}  // namespace ion

#endif
